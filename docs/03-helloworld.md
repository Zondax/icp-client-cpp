# Hello World Project

### Prerequisites

To get started with ICP-Client-C++ Hello World project, we recommend your development environment includes a hello world canister deployed locally. More information can be found [here](https://staging--eloquent-poitras-af14f0.netlify.app/docs/rust-guide/rust-quickstart.html).

### Creating the project
The complete Hello World example code can be found [here](https://github.com/Zondax/icp-client-cpp/tree/main/examples-cpp/hello).

After deploy the canister, everything is ready to start the development.
A typical C++ project structure like the next one can be created, with our library being cloned inside:

    ── hello_world
        ├── CMakeLists.txt
        ├── icp-client-cpp
        └── src
            └── main.cpp
        └── inc
            └── rust_hello_backend.did

Since we already have the Candid file we can create the C++ header for the canister.
First we compile our lib:

    cd icp-client-cpp
    mkdir build
    cd build
    cmake ..
    make

Create the header file for the canister:

    cd ic-agent-wrapper
    cargo run -p generate-cpp rust_hello_backend.did  rust_hello

The rust_hello.hpp will be created inside ic-agent-wrapper/src/declarations/rust_hello/ but it can be copied to the project include folder.

Now the agent can be configured on main.cpp:

- Define canister id (info from canister deployment), candid file location and Url;

```cpp
std::string id_text = "rrkah-fqaaa-aaaaa-aaaaq-cai";
std::string did_file = "../inc/rust_hello_backend.did";
std::string url = "http://127.0.0.1:4943";
```

- Get the candid file content to create the agent;

```cpp
auto bytes_read = did_file_content(did_file, buffer);
```

- Create Principal and Identity;

```cpp
auto principal = Principal::FromText(id_text);
Identity anonymousIdentity;
```

- Create agent and construct service;

```cpp
auto agent = Agent::create_agent(url, std::move(anonymousIdentity),
                                std::get<Principal>(principal), buffer);
SERVICE srv(std::move(std::get<Agent>(agent)));
```

- Call greet method from srv;

```cpp
std::string arg = "Zondax";
auto out = srv.greet(arg);
```

- Get the output.
```cpp
  if (out.index() == 0) {
    // Print message from the canister
    std::cout << std::get<0>(out) << std::endl;
  } else {
    // Get the second string value from the variant
    std::string str2 = std::get<1>(out);
    std::cout << "Error: " << str2 << std::endl;
  }
```

Only compilation is missing and for that we need to edit CMakeLists.txt to link the project to the library, the file should look like this:

```cpp
cmake_minimum_required(VERSION 3.15)
project(hello_world)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_LIBRARY_PATH "")
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

add_executable(helloworld src/main.cpp)

# Specify libraries  to use when linking
target_link_libraries(helloworld agent_cpp agent_c ic_agent_wrapper ${EXTRA_LIBS})
# Include deirectories with .h files
target_include_directories(helloworld PRIVATE "inc")
target_include_directories(helloworld PRIVATE "icp-client-cpp/lib-agent-cpp/inc")
target_include_directories(helloworld PRIVATE "icp-client-cpp/lib-agent-c/inc")
```
Now everything is ready to compile and run:

    mkdir build
    cd build
    cmake ..
    make
    ./helloworld

    >> Hello, Zondax!
