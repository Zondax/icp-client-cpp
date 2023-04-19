/*******************************************************************************
*   (c) 2018 - 2022 Zondax AG
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
use cty::c_int;
use ic_agent::request_id::RequestId;
use crate::RetPtr;

/// Creates a new RequestId from a SHA-256 hash.
#[no_mangle]
pub extern "C" fn request_id_new(
    bytes: *const u8,
    bytes_len: c_int,
    request_id: RetPtr<u8>) {

    let slice = unsafe {
        std::slice::from_raw_parts(bytes, bytes_len as usize)
    };
    let array = <&[u8; 32]>::try_from(slice).unwrap();
    let request_tmp = RequestId::new(array);

    let arr = request_tmp.as_slice();
    let len = arr.len() as c_int;
    request_id(arr.as_ptr(), len);
}

// Does this make sense in C ?  its just the pointer to the struture we already have
// #[no_mangle]
// pub extern "C" fn request_id_as_slice(ptr: *mut RequestId) ->  *const u8 {

// TODO : to_request_id

mod tests{
    #[allow(unused)]
    use super::*;

    #[test]
    fn test_principal_self_authenticating() {
        const HASH: [u8; 32] = [
            0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11,
            0xaa, 0x11, 0xaa, 0x11, 0xaa, 0xaa, 0x11, 0xaa, 0x11,
            0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa, 0x11, 0xaa,
            0x11, 0x11, 0xaa, 0x11, 0xaa,
        ];

        extern "C" fn request_ret(data: *const u8, len: c_int) {
            let slice = unsafe { std::slice::from_raw_parts(data, len as usize) };

            // principal management
            assert_eq!(slice, &HASH);
            assert_eq!(len as usize, HASH.len());
        }

        request_id_new(HASH.as_ptr(), HASH.len() as c_int, request_ret);
    }
}
