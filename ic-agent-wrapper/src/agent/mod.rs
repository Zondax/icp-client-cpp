/*******************************************************************************
*   (c) 2018 - 2023 Zondax AG
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/
use crate::{identity::IdentityType, AnyErr, AnyResult, CText, RetError};
use anyhow::{anyhow, bail, Context};
use candid::{
    check_prog,
    types::{Function, Type},
    IDLArgs, IDLProg, TypeEnv,
};
use cty::{c_char, c_int};
use ic_agent::export::Principal;
use ic_agent::{
    agent::status::Status,
    identity::{AnonymousIdentity, Secp256k1Identity},
    Agent, Identity,
};
use ic_utils::interfaces::management_canister::MgmtMethod;
use libc::c_void;
use std::{
    ffi::{CStr, CString},
    sync::Arc,
};
use std::{ptr, str::FromStr};
use tokio::runtime;

/// Struture that holds the information related to a specific agent
pub struct FFIAgent {
    path: String,
    identity: Arc<dyn Identity>,
    canister_id: Principal,
    did_content: String,
}

// taking in consideration a similar structure as agent unity has defined with icx info
impl FFIAgent {
    // Parse a candid file
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

    fn get_effective_canister_id(
        method_name: &str,
        args_blob: &[u8],
        canister_id: &Principal,
    ) -> anyhow::Result<Principal> {
        let is_management_canister = Principal::management_canister() == *canister_id;

        if !is_management_canister {
            Ok(*canister_id)
        } else {
            let method_name = MgmtMethod::from_str(method_name).with_context(|| {
                format!(
                    "Attempted to call an unsupported management canister method: {method_name}",
                )
            })?;

            match method_name {
                MgmtMethod::CreateCanister | MgmtMethod::RawRand => bail!(
                    "{} can only be called via an inter-canister call.",
                    method_name.as_ref()
                ),
                MgmtMethod::InstallCode => todo!(),
                MgmtMethod::StartCanister
                | MgmtMethod::StopCanister
                | MgmtMethod::CanisterStatus
                | MgmtMethod::DeleteCanister
                | MgmtMethod::DepositCycles
                | MgmtMethod::UninstallCode
                | MgmtMethod::ProvisionalTopUpCanister => {
                    let principal = Principal::from_slice(args_blob);
                    Ok(principal)
                }
                MgmtMethod::ProvisionalCreateCanisterWithCycles => {
                    Ok(Principal::management_canister())
                }
                MgmtMethod::UpdateSettings => todo!(),
                MgmtMethod::UploadChunk => todo!(),
                MgmtMethod::ClearChunkStore => todo!(),
                MgmtMethod::StoredChunks => todo!(),
                MgmtMethod::InstallChunkedCode => todo!(),
                MgmtMethod::FetchCanisterLogs => todo!(),
                MgmtMethod::TakeCanisterSnapshot => todo!(),
                MgmtMethod::DeleteCanisterSnapshot => todo!(),
                MgmtMethod::EcdsaPublicKey => todo!(),
                MgmtMethod::SignWithEcdsa => todo!(),
                MgmtMethod::BitcoinGetBalance => todo!(),
                MgmtMethod::BitcoinGetUtxos => todo!(),
                MgmtMethod::BitcoinSendTransaction => todo!(),
                MgmtMethod::LoadCanisterSnapshot => todo!(),
                MgmtMethod::ListCanisterSnapshots => todo!(),
                MgmtMethod::NodeMetricsHistory => todo!(),
                MgmtMethod::CanisterInfo => todo!(),
                MgmtMethod::BitcoinGetCurrentFeePercentiles => todo!(),
                MgmtMethod::BitcoinGetBlockHeaders => todo!(),
            }
        }
    }

    // Create real agent based on FFIAgent
    pub async fn inner_ic_create(&self) -> AnyResult<Agent> {
        let agent_tmp = ic_agent::Agent::builder()
            .with_url(&self.path)
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

    // Update Call directly from the ic agent
    pub async fn inner_ic_update(&self, method: &str, method_args: &str) -> AnyResult<IDLArgs> {
        let (ty_env, actor) = self.inner_parse_candid_file()?;
        let func_sig = Self::get_method_signature(method, &ty_env, &actor)?;

        let args_blb = Self::inner_blob_from_raw(method_args, &ty_env, &func_sig)?;

        let effective_canister_id =
            Self::get_effective_canister_id(method, args_blb.as_slice(), &self.canister_id)?;

        let agent = self.inner_ic_create().await?;

        let rst_blb = agent
            .update(&self.canister_id, method)
            .with_arg(args_blb)
            .with_effective_canister_id(effective_canister_id)
            .call_and_wait()
            .await
            .map_err(AnyErr::from)?;

        let rst_idl = Self::idl_from_blob(rst_blb.as_slice(), &ty_env, &func_sig)?;
        Ok(rst_idl)
    }

    // Query Call directly from the ic agent
    pub async fn inner_ic_query(&self, method: &str, method_args: &str) -> AnyResult<IDLArgs> {
        let (ty_env, actor) = self.inner_parse_candid_file()?;
        let func_sig = Self::get_method_signature(method, &ty_env, &actor)?;

        let args_blb = Self::inner_blob_from_raw(method_args, &ty_env, &func_sig)?;

        let effective_canister_id =
            Self::get_effective_canister_id(method, args_blb.as_slice(), &self.canister_id)?;

        let agent = self.inner_ic_create().await?;

        let rst_blb = agent
            .query(&self.canister_id, method)
            .with_arg(args_blb)
            .with_effective_canister_id(effective_canister_id)
            .call()
            .await
            .map_err(AnyErr::from)?;

        let rst_idl = Self::idl_from_blob(rst_blb.as_slice(), &ty_env, &func_sig)?;

        Ok(rst_idl)
    }

    fn inner_blob_from_raw(
        args_raw: &str,
        ty_env: &TypeEnv,
        meth_sig: &Function,
    ) -> AnyResult<Vec<u8>> {
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

/// @brief Creates a FFIAgent instance to be used on the remaining agent functions
///
/// @param path Pointer to array of bytes
/// @param identity Pointer to identity
/// @param id_type Identity Type
/// @param canister_id Pointer to Principal Canister Id
/// @param canister_id_len Length of Principal ID
/// @param did_content Content of .did file
/// @param error_ret CallBack to get error
/// @return Pointer to FFIAgent structure
/// If the function returns a NULL pointer the user should check
/// The error callback, to attain the error
#[no_mangle]
pub extern "C" fn agent_create_wrap(
    path: *const c_char,
    identity: *const c_void,
    id_type: IdentityType,
    canister_id: *const u8,
    canister_id_len: c_int,
    did_content: *const c_char,
    error_ret: Option<&mut RetError>,
) -> *mut FFIAgent {
    let computation = || -> AnyResult<FFIAgent> {
        let path = unsafe { CStr::from_ptr(path).to_str().map_err(AnyErr::from) }?.to_string();
        let did_content =
            unsafe { CStr::from_ptr(did_content).to_str().map_err(AnyErr::from) }?.to_string();

        let slice = unsafe { std::slice::from_raw_parts(canister_id, canister_id_len as usize) };
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

        Ok(FFIAgent {
            path,
            identity,
            canister_id,
            did_content,
        })
    };

    match computation() {
        Ok(agent) => Box::into_raw(Box::new(agent)),
        Err(e) => {
            let err_str = e.to_string();
            let c_string = CString::new(err_str.clone()).unwrap_or_else(|_| {
                let fallback_error = "Failed to convert error message to CString";
                CString::new(fallback_error).expect("Fallback error message is invalid")
            });
            if let Some(error_ret) = error_ret {
                (error_ret.call)(
                    c_string.as_ptr() as _,
                    c_string.as_bytes().len() as _,
                    error_ret.user_data,
                );
            }

            ptr::null_mut()
        }
    }
}

/// @brief Calls and returns the information returned by the status endpoint of a replica
///
/// @param agent_ptr Pointer to FFI structure that holds agent info
/// @param error_ret CallBack to get error
/// @return Pointer to FFIAgent structure
/// If the function returns a NULL CText the user should check
/// The error callback, to attain the error
#[no_mangle]
pub extern "C" fn agent_status_wrap(
    agent_ptr: Option<&FFIAgent>,
    error_ret: Option<&mut RetError>,
) -> Option<Box<CText>> {
    let computation = || -> AnyResult<_> {
        let agent = agent_ptr.ok_or(anyhow!("FFIAgent instance null"))?;

        let runtime = runtime::Runtime::new()?;
        let status = runtime.block_on(agent.inner_ic_status())?;

        let status_str = status.to_string();

        Ok(status_str)
    };

    match computation() {
        Ok(status_str) => {
            let data = CString::new(status_str).ok()?.into_bytes();
            let c_text = Box::new(CText { data });
            Some(c_text)
        }
        Err(e) => {
            let err_str = e.to_string();
            let c_string = CString::new(err_str.clone()).unwrap_or_else(|_| {
                let fallback_error = "Failed to convert error message to CString";
                CString::new(fallback_error).expect("Fallback error message is invalid")
            });
            if let Some(error_ret) = error_ret {
                (error_ret.call)(
                    c_string.as_ptr() as _,
                    c_string.as_bytes().len() as _,
                    error_ret.user_data,
                );
            }
            None
        }
    }
}

/// @brief Calls and returns a query call to the canister
///
/// @param agent_ptr Pointer to FFI structure that holds agent info
/// @param method Pointer service/method name from did information
/// @param method_args Pointer to the arguments required by method
/// @param error_ret CallBack to get error
/// @return Pointer to IDLArgs
#[no_mangle]
pub extern "C" fn agent_query_wrap(
    agent_ptr: Option<&FFIAgent>,
    method: *const c_char,
    method_args: *const c_char,
    error_ret: Option<&mut RetError>,
) -> *mut IDLArgs {
    let computation = || -> AnyResult<_> {
        let agent = agent_ptr.ok_or(anyhow!("FFIAgent instance null"))?;

        let method = unsafe { CStr::from_ptr(method).to_str().map_err(AnyErr::from) }?;
        let method_args = unsafe { CStr::from_ptr(method_args).to_str().map_err(AnyErr::from) }?;

        let runtime = runtime::Runtime::new()?;
        let rst_idl = runtime.block_on(agent.inner_ic_query(method, method_args))?;

        Ok(rst_idl)
    };

    match computation() {
        Ok(idl) => Box::into_raw(Box::new(idl)) as *mut IDLArgs,
        Err(e) => {
            let err_str = e.to_string();
            let c_string = CString::new(err_str.clone()).unwrap_or_else(|_| {
                let fallback_error = "Failed to convert error message to CString";
                CString::new(fallback_error).expect("Fallback error message is invalid")
            });
            if let Some(error_ret) = error_ret {
                (error_ret.call)(
                    c_string.as_ptr() as _,
                    c_string.as_bytes().len() as _,
                    error_ret.user_data,
                );
            }
            ptr::null_mut()
        }
    }
}

/// @brief Calls and returns a update call to the canister
///
/// @param agent_ptr Pointer to FFI structure that holds agent info
/// @param method Pointer service/method name from did information
/// @param method_args Pointer to the arguments required by method
/// @param error_ret CallBack to get error
/// @return Pointer to IDLArgs
/// If the function returns a NULL CText the user should check
/// The error callback, to attain the error
#[no_mangle]
pub extern "C" fn agent_update_wrap(
    agent_ptr: Option<&FFIAgent>,
    method: *const c_char,
    method_args: *const c_char,
    error_ret: Option<&mut RetError>,
) -> *mut IDLArgs {
    let computation = || -> AnyResult<_> {
        let agent = agent_ptr.ok_or(anyhow!("FFIAgent instance null"))?;
        let method = unsafe { CStr::from_ptr(method).to_str().map_err(AnyErr::from) }?;
        let method_args = unsafe { CStr::from_ptr(method_args).to_str().map_err(AnyErr::from) }?;

        let runtime = runtime::Runtime::new()?;
        let rst_idl = runtime.block_on(agent.inner_ic_update(method, method_args))?;

        Ok(rst_idl)
    };

    match computation() {
        Ok(idl) => Box::into_raw(Box::new(idl)) as *mut IDLArgs,
        Err(e) => {
            let err_str = e.to_string();
            let c_string = CString::new(err_str.clone()).unwrap_or_else(|_| {
                let fallback_error = "Failed to convert error message to CString";
                CString::new(fallback_error).expect("Fallback error message is invalid")
            });
            if let Some(error_ret) = error_ret {
                (error_ret.call)(
                    c_string.as_ptr() as _,
                    c_string.as_bytes().len() as _,
                    error_ret.user_data,
                );
            }
            ptr::null_mut()
        }
    }
}

/// @brief Free allocated Agent
///
/// @param agent_ptr Pointer to FFI structure that holds agent info
#[no_mangle]
pub extern "C" fn agent_destroy(_agent: Option<Box<FFIAgent>>) {}

#[cfg(test)]
mod tests {
    #[allow(unused)]
    use super::*;
    use crate::identity::identity_anonymous;

    const IC_PATH: &[u8] = b"http://127.0.0.1:4943\0";
    const II_DID_CONTENT_BYTES: &[u8] =
        const_str::concat_bytes!(include_bytes!("rust_hello_backend.did"), b"\0");
    const II_CANISTER_ID_BYTES: &[u8] = &[0, 0, 0, 0, 0, 0, 0, 1, 1, 1];

    fn cbytes_to_str(cbytes: &[u8]) -> &str {
        let cstr = CStr::from_bytes_with_nul(cbytes).unwrap();
        cstr.to_str().unwrap()
    }

    #[test]
    fn test_agent_create_with_anonymous() {
        let identity = identity_anonymous();

        let agent = agent_create_wrap(
            IC_PATH.as_ptr() as *const c_char,
            identity,
            IdentityType::Anonym,
            II_CANISTER_ID_BYTES.as_ptr(),
            II_CANISTER_ID_BYTES.len() as i32,
            II_DID_CONTENT_BYTES.as_ptr() as *mut c_char,
            None,
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
            assert_eq!(agent_ptr.did_content, cbytes_to_str(II_DID_CONTENT_BYTES));
        }
    }

    #[test]
    fn test_agent_query() {
        const EXPECTED: &str = "(\"Hello, World!\")";
        let identity = identity_anonymous();

        let agent = agent_create_wrap(
            IC_PATH.as_ptr() as *const c_char,
            identity,
            IdentityType::Anonym,
            II_CANISTER_ID_BYTES.as_ptr(),
            II_CANISTER_ID_BYTES.len() as i32,
            II_DID_CONTENT_BYTES.as_ptr() as *mut c_char,
            None,
        );

        let ret = agent_query_wrap(
            unsafe { agent.as_ref() },
            b"greet\0".as_ptr() as *const c_char,
            b"(\"World\")\0".as_ptr() as *const c_char,
            None,
        );

        unsafe {
            let idl_boxed = Box::from_raw(ret as *mut IDLArgs);
            assert_eq!(EXPECTED, idl_boxed.to_string());
        }
    }

    #[test]
    fn test_agent_update() {
        const EXPECTED: &str = "(\"Hello, World!\")";
        let identity = identity_anonymous();

        let agent = agent_create_wrap(
            IC_PATH.as_ptr() as *const c_char,
            identity,
            IdentityType::Anonym,
            II_CANISTER_ID_BYTES.as_ptr(),
            II_CANISTER_ID_BYTES.len() as i32,
            II_DID_CONTENT_BYTES.as_ptr() as *mut c_char,
            None,
        );

        let ret = agent_update_wrap(
            unsafe { agent.as_ref() },
            b"greet\0".as_ptr() as *const c_char,
            b"(\"World\")\0".as_ptr() as *const c_char,
            None,
        );

        unsafe {
            let idl_boxed = Box::from_raw(ret as *mut IDLArgs);
            assert_eq!(EXPECTED, idl_boxed.to_string());
        }
    }

    #[test]
    fn test_agent_status() {
        let identity = identity_anonymous();

        let agent = agent_create_wrap(
            IC_PATH.as_ptr() as *const c_char,
            identity,
            IdentityType::Anonym,
            II_CANISTER_ID_BYTES.as_ptr(),
            II_CANISTER_ID_BYTES.len() as i32,
            II_DID_CONTENT_BYTES.as_ptr() as *mut c_char,
            None,
        );

        let _status = agent_status_wrap(unsafe { agent.as_ref() }, None);

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
            assert_eq!(agent_ptr.did_content, cbytes_to_str(II_DID_CONTENT_BYTES));
        }
    }
}
