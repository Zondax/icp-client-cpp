# IC-C

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![GithubActions](https://github.com/zondax/icp-client-cpp/actions/workflows/build.yml/badge.svg)](https://github.com/Zondax/icp-client-cpp/blob/main/.github/workflows/build.yaml)

---

![zondax_light](docs/zondax_light.png#gh-light-mode-only)
![zondax_dark](docs/zondax_dark.png#gh-dark-mode-only)

_Please visit our website at [zondax.ch](https://www.zondax.ch)_

---

## :warning: ATTENTION

Please, **Do not use yet in production**.

## General Description

The objective of this project is to provide a C and C++ interface for the Rust Agent Crate

Reference: https://internetcomputer.org/docs/current/developer-docs/agents/

What an agent does:

- Encodes arguments to be sent in REST calls (Candid based)
- Verifies certificates and decode responses
- Manages authentication

### Disclaimer
Team is currently working on building the project and running it on Windows platforms.

### Project Build and Description

Configure Project and generate makefile.

    mkdir build
    cd build
    cmake ..
    make

After compilation, the library offers three primary functionalities:

- C Wrapper for the agent: A static library (libagent_c.a) that provides a C wrapper for the agent.
- C++ Interface for the agent  (libagent_cpp.a) : Another static library that uses the C wrapper to provide a C++ interface for the agent.
- Cpp Header File Generator from Candid File: This feature simplifies developer interaction with a canister by generating an hpp file from the canister candid file. This eliminates the need to handle candid types directly.
All candid services are translated into C++ functions using native C++ types, allowing them to be called with the C++ interface. This makes it more convenient for developers to work with the library and interact with canisters.

To use the generator:

    cd ic-agent-wrapper
    cargo run -p generate-cpp {did file} {canister name}

The hpp file can be found on the root of the project inside
/src/declarations/{canister_name}/.

### Guidance & Core Testing 

The testing framework [doctest](https://github.com/doctest/doctest/tree/master) is used for unit testing different functionality exported by this library.
The tests can be compiled by running:
```
make tests 
```
This command will generate a new ***test*** binary which can be executed:

```
./test

```
The framework will run all the tests and generate a report similar to the one bellow:
```
╰─ ./test                                                                                                                                                        ─╯
[doctest] doctest version is "2.4.11"
[doctest] run with "--help" for options
===============================================================================
[doctest] test cases:  8 |  8 passed | 0 failed | 0 skipped
[doctest] assertions: 20 | 20 passed | 0 failed |
[doctest] Status: SUCCESS!
```
You can list the different available tests in the library by running:
```
./test -ltc 
```
The output at the moment of writting is: 
```
╰─ ./test -ltc                                                                                                                                                   ─╯
[doctest] listing all test case names
===============================================================================
Identity_from_pem
Anonymous Identity
Testing Anonymous Principal
Testing Anonymous Principal from/to text
Principal from bytes
Principal from bytes should fail
Principal SelfAuthenticating
Principal management
===============================================================================
[doctest] unskipped test cases passing the current filters: 8
```
Please refer to doctest documentation and available options.

On the examples folders it can be found different usage examples and
testing examples for the core exposed functions. All the examples are compiled with the projects and the executables can be found on hte build/ folder. The main examples that can be used as guidance are:

- examples/hello_c : interaction with a caniter using c wrapper, requires deploying a local hello world canister (see more below);

- examples-cpp/hello : interaction with a caniter using Cpp interface, canister header file already generated requires deploying a local hello world canister (see more below);

- examples/ic_c : interaction with IC caniter using c wrapper.

- examples-cpp/ic : interaction with a caniter using Cpp interface, canister header file already generated.


### How to use

To use this library in a project you can clone this source code into your project and build the library with:

    mkdir build
    cd build
    cmake ..
    make

Create the header file for the canister:

    cd ic-agent-wrapper
    cargo run -p generate-cpp ../rust_hello_backend.did  rust_hello

Take as an example a simple hello world project with the following structure:

    ── hello_world
        ├── CMakeLists.txt
        ├── icp-client-cpp
        └── src
            └── main.cpp
        └── inc
            ├── rust_hello_backend.did
            └── rust_hello.hpp
        

Then link the rust wrapper library, ic_agent_wrapper, and the c library, agent-c.
For a CMake platform, the Cmakelists file would look like this:

    # OS-specific libraries
    if(APPLE)
        find_library(SECURITY_LIB Security)
        find_library(COREFOUNDATION_LIB CoreFoundation)
        set(EXTRA_LIBS ${SECURITY_LIB} ${COREFOUNDATION_LIB})
    elseif(UNIX AND NOT APPLE)
        set(EXTRA_LIBS m)
    elseif(WIN32)
        # Add any Windows-specific libraries here
    endif()

    # Link against the icp-client-cpp libraries
    link_directories(icp-client-cpp/build)
    link_directories(icp-client-cpp/ic-agent-wrapper/target/release/)

    add_executable(helloworld src/main.c)

    # Specify libraries  to use when linking
    target_link_libraries(helloworld agent_cpp agent_c ic_agent_wrapper ${EXTRA_LIBS})
    # Include deirectories with .h files
    target_include_directories(helloworld PRIVATE "inc")
    target_include_directories(helloworld PRIVATE "icp-client-cpp/lib-agent-cpp/inc")
    target_include_directories(helloworld PRIVATE "icp-client-cpp/lib-agent-c/inc")

#### Deploy local Hello world canister

In a separate directory create new hello world canister:

    dfx new --type=rust rust_hello

Change to your project directory:

    cd rust_hello

Start the local execution environment:

    dfx start --background

Register, build, and deploy the canister:

    dfx deploy

### Rust agent crate

- Repository: https://github.com/dfinity/agent-rs
- Documentation: https://docs.rs/ic-agent/latest/ic_agent/
