use std::{ffi::{CStr, CString}, sync::Arc};
use anyhow::{anyhow, bail};
use candid::{IDLArgs, TypeEnv, IDLProg, check_prog, types::{Type, Function}, Principal};
use cty::{c_char, c_void, c_int};
use ic_agent::{identity::{AnonymousIdentity, Secp256k1Identity}, Identity, agent::{http_transport::ReqwestHttpReplicaV2Transport, status::Status}, Agent};
use tokio::runtime;
use crate::{identity::IdentityType, RetPtr, ResultCode, AnyErr, AnyResult};

pub struct FFIAgent {
    path: String,
    identity:  Arc<dyn Identity>,
    canister_id: Principal,
    did_content: String,
}

// taking in consideration a similar struture as agent unity has defined
impl FFIAgent {

    // PArse a cadndid file
    fn inner_parse_candid_file(&self) -> AnyResult<(TypeEnv, Option<Type>)> {
        let ast = self.did_content.parse::<IDLProg>().map_err(AnyErr::from)?;

        let mut env = TypeEnv::new();
        let actor = check_prog(&mut env, &ast).map_err(AnyErr::from)?;

        Ok((env, actor))
    }
    
    fn get_method_signature(
        method_name: &str,
        ty_env: &TypeEnv,
        actor: &Option<Type>,
    ) -> AnyResult<Function> {
        match actor {
            Some(actor) => {
                let method_sig = ty_env
                    .get_method(actor, method_name)
                    .map_err(AnyErr::from)?
                    .clone();

                Ok(method_sig)
            }
            None => bail!("Failed to get method: {}", method_name),
        }
    }
    // Create real agent based on FFIAgent
    pub async fn inner_ic_create(&self) -> AnyResult<Agent> {

        let transport = ReqwestHttpReplicaV2Transport::create(&self.path).map_err(AnyErr::from)?;
        let agent_tmp = Agent::builder()
            .with_transport(transport)
            .with_arc_identity(self.identity.clone())
            .build()
            .map_err(AnyErr::from)?;

        agent_tmp.fetch_root_key().await.map_err(AnyErr::from)?;

        Ok(agent_tmp)
    }

    // Get status directly from the ic agent
    pub async fn inner_ic_status(&self) -> AnyResult<Status> {

    let agent = self.inner_ic_create().await?;
    agent.status().await.map_err(AnyErr::from)
    }

    // Query Call directly from the ic agent
    pub async fn inner_ic_query(
        &self,
        method: &str,
        method_args: &str) -> AnyResult<IDLArgs> {

        let (ty_env, actor) = self.inner_parse_candid_file()?;
        let func_sig = Self::get_method_signature(method, &ty_env, &actor)?;

        let args_blb = Self::inner_blob_from_raw(method_args, &ty_env, &func_sig)?;

        let agent = self.inner_ic_create().await?;

        let rst_blb = agent
        .query(&self.canister_id, method)
        .with_arg(args_blb)
        .with_effective_canister_id(Principal::management_canister())
        .call()
        .await
        .map_err(AnyErr::from)?;

        let rst_idl = Self::idl_from_blob(rst_blb.as_slice(), &ty_env, &func_sig)?;

        Ok(rst_idl)
    }

    fn inner_blob_from_raw(args_raw: &str, ty_env: &TypeEnv, meth_sig: &Function) -> AnyResult<Vec<u8>> {
        let args_idl = args_raw.parse::<IDLArgs>().map_err(AnyErr::from)?;

        let args_blob = args_idl
            .to_bytes_with_types(ty_env, &meth_sig.args)
            .map_err(AnyErr::from)?;

        Ok(args_blob)
    }

    fn idl_from_blob(args_blb: &[u8], ty_env: &TypeEnv, meth_sig: &Function) -> AnyResult<IDLArgs> {
        IDLArgs::from_bytes_with_types(args_blb, ty_env, &meth_sig.rets).map_err(AnyErr::from)
    }
}



/// Creates a FFIAgent instance to be used on the remaining agent functions
#[no_mangle]
pub extern "C" fn agent_create(
    path: *const c_char,
    identity: *const c_void,
    id_type: IdentityType,
    canister_id_content: *const u8,
    canister_id_len: c_int,
    did_content: *const c_char,
    agent_ptr: *mut *const FFIAgent,
    error_ret: RetPtr<u8>,
) -> ResultCode {
    let computation = || -> AnyResult<FFIAgent> {
        let path = unsafe {CStr::from_ptr(path).to_str().map_err(AnyErr::from)}?.to_string();
        let did_content = unsafe {CStr::from_ptr(did_content).to_str().map_err(AnyErr::from)}?.to_string();
        
        let slice = unsafe {
            std::slice::from_raw_parts(canister_id_content, canister_id_len as usize)
        };
        let canister_id = Principal::from_slice(slice);

        let identity = unsafe {
            match id_type {
                IdentityType::Anonym => Arc::new(AnonymousIdentity {}) as Arc<dyn Identity>,
                IdentityType::Secp256k1 => {
                    let ptr = identity as *const Secp256k1Identity;
                    let boxed = Box::from_raw(ptr as *mut Secp256k1Identity);
                    let cloned = Arc::<Secp256k1Identity>::from(boxed.clone()) as Arc<dyn Identity>;
                    Box::into_raw(boxed);
                    cloned
                }
                IdentityType::Basic => {
                    // Basic cant be cloned?
                    // let ptr = identity as *const BasicIdentity;
                    // let boxed = Box::from_raw(ptr as *mut BasicIdentity);
                    // let cloned = Arc::<BasicIdentity>::from(boxed.clone()) as Arc<dyn Identity>;
                    // Box::into_raw(boxed);
                    return Err(anyhow!("Missing attribute"));

                }

            }
        };

        Ok(FFIAgent {path, identity, canister_id, did_content})
    };

    match computation() {
        Ok(agent) => {
            let agent_tmp = Box::into_raw(Box::new(agent));
            unsafe {
                *agent_ptr = agent_tmp;
            }
            ResultCode::Ok
        }
        Err(e) =>{
            let err_str = e.to_string() + "\0";
            let arr = err_str.as_bytes();
            let len = arr.len() as c_int;
            error_ret(arr.as_ptr(), len);
            ResultCode::Err
        }
    }
}

/// Calls and returns the information returned by the status endpoint of a replica.
#[no_mangle]
pub extern "C" fn agent_status(
    agent_ptr: *const FFIAgent,
    status_ret: RetPtr<u8>,
    error_ret: RetPtr<u8>,
) -> ResultCode {
    let computation = || -> AnyResult<_> {
        let agent = unsafe { Box::from_raw(agent_ptr as *mut FFIAgent) };

        let runtime = runtime::Runtime::new()?;
        let status = runtime.block_on(agent.inner_ic_status())?;

        // Don't drop the [`AgentWrapper`]
        Box::into_raw(agent);

        let status_cstr = CString::new(status.to_string())
            .map_err(AnyErr::from)?
            .into_bytes_with_nul();

        Ok(status_cstr)
    };

    match computation() {
        Ok(status_cstr) => {
            let arr = status_cstr.as_slice();
            let len = arr.len() as c_int;
            status_ret(arr.as_ptr(), len);
            ResultCode::Ok
        }
        Err(e) =>{
            let err_str = e.to_string() + "\0";
            let arr = err_str.as_bytes();
            let len = arr.len() as c_int;
            error_ret(arr.as_ptr(), len);
            ResultCode::Err
        }
    }
}

#[no_mangle]
pub extern "C" fn agent_query(
    agent_ptr: *const FFIAgent,
    method: *const c_char,
    method_args: *const c_char,
    ret: *mut *const c_void,
    error_ret: RetPtr<u8>,
) -> ResultCode {
    let computation = || -> AnyResult<_> {
        let agent = unsafe { Box::from_raw(agent_ptr as *mut FFIAgent) };
        let method = unsafe { CStr::from_ptr(method).to_str().map_err(AnyErr::from) }?;
        let method_args = unsafe { CStr::from_ptr(method_args).to_str().map_err(AnyErr::from) }?;

        let runtime = runtime::Runtime::new()?;
        let rst_idl = runtime.block_on(agent.inner_ic_query(method, method_args))?;

        Ok(rst_idl)
    };

    match computation() {
        Ok(idl) => {
            let idl_ptr: *const c_void = Box::into_raw(Box::new(idl)) as *const c_void;
            unsafe {
                *ret = idl_ptr;
            }
            ResultCode::Ok
        }
        Err(e) =>{
            let err_str = e.to_string() + "\0";
            let arr = err_str.as_bytes();
            let len = arr.len() as c_int;
            error_ret(arr.as_ptr(), len);
            ResultCode::Err
        }
    }
}

#[cfg(test)]
mod tests {
     #[allow(unused)]
    use super::*;
    use crate::identity::{identity_anonymous};
    use libc::c_int;

    const IC_PATH: &[u8] = b"https://ic0.app\0";
    const II_CANISTER_ID_BYTES: &[u8] = &[0, 0, 0, 0, 0, 0, 0, 7, 1, 1];
    const II_DID_CONTENT_BYTES: &[u8] = b"rust_hello_backend.did\0";

    fn cbytes_to_str(cbytes: &[u8]) -> &str {
        let cstr = CStr::from_bytes_with_nul(cbytes).unwrap();
        cstr.to_str().unwrap()
    }

    extern "C" fn error_ret(_data: *const u8, _len: c_int) {}

    #[test]
    fn test_agent_create_with_anonymous() {
        let mut identity: *const c_void = std::ptr::null();
        identity_anonymous(&mut identity);
        let mut agent: *const FFIAgent = std::ptr::null();

        assert_eq!(
            agent_create(
                IC_PATH.as_ptr() as *const c_char,
                identity,
                IdentityType::Anonym,
                II_CANISTER_ID_BYTES.as_ptr(),
                II_CANISTER_ID_BYTES.len() as c_int,
                II_DID_CONTENT_BYTES.as_ptr() as *const c_char,
                &mut agent,
                error_ret
            ),
            ResultCode::Ok
        );

        unsafe {
            let id = Box::from_raw(identity as *mut AnonymousIdentity);
            assert!(id.sender().is_ok());

            let agent_ptr = Box::from_raw(agent as *mut FFIAgent);
            assert_eq!(agent_ptr.path, cbytes_to_str(IC_PATH));
            assert_eq!(agent_ptr.identity.sender(), id.sender());
            assert_eq!(
                agent_ptr.canister_id,
                Principal::from_slice(II_CANISTER_ID_BYTES)
            );
            assert_eq!(
                agent_ptr.did_content,
                cbytes_to_str(II_DID_CONTENT_BYTES)
            );
        }
    }
}
