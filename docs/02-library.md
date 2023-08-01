# Library Walkthrough

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

### C++ Header Generator

The C++ Header Generator tool is a tool available in this project that allows the user to get a C++ header file from a candid file.
All the services from the candid file and translate them to C++ functions that use our C++ library to handle the Candid types as C++ native types.

Lets take as an example this Candid file :

    service : {
    "increment": () -> ();
    "get": () -> (nat64) query;
    "set": (nat64) -> ();
    }

The C++ header will have 3 functions that will use a uint64 instead of the nat64:
    
```cpp
std::variant<uint64_t, std::string>get()
std::variant<std::monostate, std::string>increment()
std::variant<std::monostate, std::string>set(uint64_t arg0)
```

This functions belong to SERVICE class, and the user can use this class constructor
to initialize a SERVICE instance with the agent configured previously with Agent::create_agent interface for the C++ lib.

To use the generator:

    cd ic-agent-wrapper
    cargo run -p generate-cpp {did file} {canister name}

The hpp file can be found inside
ic-agent-wrapper/src/declarations/{canister_name}/.

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
The output at the moment of writing is: 
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

- examples/hello_c : interaction with a canister using c wrapper, requires deploying a local hello world canister (see more below);

- examples-cpp/hello : interaction with a canister using Cpp interface, canister header file already generated requires deploying a local hello world canister (see more below);

- examples/ic_c : interaction with IC canister using c wrapper.

- examples-cpp/ic : interaction with a canister using Cpp interface, canister header file already generated.
