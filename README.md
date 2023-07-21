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

The objective of this project is to provide a C wrapper for the Rust Agent Crate

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

#### ic-agent-wrapper

Contains rust code to expose ic-agent lib to C.

#### lib-agent-c

Library folder where we use the wrapper exposed functions
to create C friendly functions to be used. It is compiled as a static library.

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

On the example folder it can be found different usage examples and
testing examples for the core exposed functions. 

After the project is built, with the previous commands, there will be available on the build folder
different binaries:

- hello-icp: simple example that requires deploying a local hello world canister (see more below);
- icp-app: interact with canister running on icp0.app, getting a result for a lookup command and extracting the first IDLValue from the result vector;
- candid: usage tests and examples for the exported function from the candid module;

### How to use it

To use this library in a project you can clone this source code into your project and build the library with:

    mkdir build
    cd build
    cmake ..
    make

Take as an example a simple hello world project with the following structure:

    ── hello_world
        ├── CMakeLists.txt
        ├── icp-client-cpp
        │   ├── CMakeLists.txt
        │   ├── build
        │   │   └-─ libagent_c.a
        │   ├── ic-agent-wrapper
        │   │   └── target
        │   │       └── release
        │   │           └─── libic_agent_wrapper.a
        │   └── lib-agent-c
        │       └── inc
        │           ├── agent.h
        │           ├── bindings.h
        │           ├── helper.h
        │           └── identity.h
        └── src
            └── main.c

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
    target_link_libraries(helloworld agent_c ic_agent_wrapper ${EXTRA_LIBS})
    # Include directories with .h files
    target_include_directories(helloworld PRIVATE "icp-client-cpp/lib-agent-c/inc")


Header description:
- bindings.h : contains all the signatures and documentation for the functions exposed from the ic-agent crate
- agent.h : uses exported agent related functions to offer a C friendly interface to interact with the agent
- identity.h : uses exported identity related functions to offer a C friendly interface to get identity
- principal.h : uses exported principal related functions to offer a C friendly interface
- helper.h : functions to read the content from .did file and helper structures

### Running Hello World example

#### 1. Deploy local Hello world canister

In a separate directory create new hello world canister:

    dfx new --type=rust rust_hello

Change to your project directory:

    cd rust_hello

Start the local execution environment:

    dfx start --background

Register, build, and deploy the canister:

    dfx deploy

#### 2. Use IC-C agent to interact with local canister

Inside IC-C folder configure Project and generate makefile:

    cmake .

Compile and link project:

    cmake --build .

Run hello_world example:

    ./hello_icp

The example sends the text "World" to the available canister service "greet", the response
is represented inside the () :

    Hello ICP! 
    ("Hello, World!")

### Rust agent crate

- Repository: https://github.com/dfinity/agent-rs
- Documentation: https://docs.rs/ic-agent/latest/ic_agent/
