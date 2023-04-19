# IC-C

## Project Build

Configure Project and generate makefile.

    cmake .

Compile and link project

    cmake --build .

Run hello_world example (deploy canister first):

    ./hello_icp

#### ic-agent-wrapper

Contains rust code to expose ic-agent lib to C.

#### lib-agent-c

Library folder where we use the wrapper exposed functions
to create C friendly functions to be used. It is compiled as a static library.

#### Deploy Hello world canister
To create a template project with a hello world canister example run in a new folder:

    dfx new --type=rust rust_hello

Start the local execution environment:

    dfx start --background

Deploy the Canister:

    dfx deploy

## General Description
This is just a proof of concept.

The objective of this project is to provide a C/C++ wrapper for the Rust Agent Crate

Reference: https://internetcomputer.org/docs/current/developer-docs/agents/

What an agent does:
- Encode arguments to be sent in REST calls (Candid based)
- Verify certificates and decode responses
- Manage authentication

Our objective is to:
- expose this functionality to C/C++ users
- Ensure that this works in Linux, macOS and Windows (use CI)

The Rust code exposes the following functionality:

https://docs.rs/ic-agent/latest/ic_agent/all.html

Ideally, we should be able to:
- Follow this to expose things https://docs.rust-embedded.org/book/interoperability/rust-with-c.html
- Then use cbindgen to generate headers https://github.com/eqrion/cbindgen
- Lastly, use this result to write a few friendlier wrappers + CI tests + examples.
This will allow other people to understand how to use this.

Candid can be a challenge, but as discussed with ICP team, we can first expose a simpler minimalistic interface.

:warning: ICP has indicated that their preferred outcome is that we provide a C wrapper (not C++).

## References

### Other articles and good references

https://developers.redhat.com/articles/2022/09/05/how-create-c-binding-rust-library#output_pointer_for_a_string

https://apollolabsblog.hashnode.dev/rust-ffi-and-cbindgen-integrating-embedded-rust-code-in-c

https://slint-ui.com/blog/expose-rust-library-to-other-languages.html#:~:text=Rust%20cannot%20expose%20a%20C%2B%2B,they%20have%20a%20C%2B%2B%20equivalent.

https://doc.rust-lang.org/nomicon/ffi.html

### Rust agent crate
- Repository: https://github.com/dfinity/agent-rs
- Documentation: https://docs.rs/ic-agent/latest/ic_agent/
