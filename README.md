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

During Milestone 1, we concentrated on studying and understanding 
the full scope of ICP network and also achieving a functional hello world 
example to test the acquired knowledge and prove the concept and usability of a
C wrapper for the rust agent.

For Milestone 2, the team will iterate over library code and structure, and improve
library usability. Namely, code consistency, memory management improvements and
removing the need for global variables.

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

### Project Build and Description

Configure Project and generate makefile.

    cmake .

Compile and link project

    cmake --build .

#### ic-agent-wrapper

Contains rust code to expose ic-agent lib to C.

#### lib-agent-c

Library folder where we use the wrapper exposed functions
to create C friendly functions to be used. It is compiled as a static library.


### Rust agent crate

- Repository: https://github.com/dfinity/agent-rs
- Documentation: https://docs.rs/ic-agent/latest/ic_agent/
