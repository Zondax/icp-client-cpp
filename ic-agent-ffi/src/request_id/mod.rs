use ic_agent::request_id::RequestId;

/// Creates a new RequestId from a SHA-256 hash.
#[no_mangle]
pub extern "C" fn request_id_new(hash:  &[u8; 32]) -> *mut RequestId {
    Box::into_raw(Box::new(RequestId::new(hash)))
}

/// Free created RequestId
#[no_mangle]
pub extern "C" fn request_id_free(client: *mut RequestId) {
    if !client.is_null() {
        unsafe {
            drop(Box::from_raw(client));
        }
    }
}

/// Returns the SHA-256 hash this ID is based on.
#[no_mangle]
pub extern "C" fn request_id_as_slice(ptr: *mut RequestId) ->  *const u8 {
    let mpc = unsafe {
        assert!(!ptr.is_null());
        &mut *ptr
    };
    mpc.as_slice().as_ptr()
}
