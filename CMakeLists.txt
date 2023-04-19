cmake_minimum_required(VERSION 3.10)
project(ICP-C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_LIBRARY_PATH "")

# OS-specific libraries
if(APPLE)
    find_library(SECURITY_LIB Security)
    find_library(COREFOUNDATION_LIB CoreFoundation)
    set(EXTRA_LIBS ${SECURITY_LIB} ${COREFOUNDATION_LIB})
endif()

# Compile Rust library in ic-agent-wrapper using cargo build
add_custom_target(
        ic_agent_wrapper_cargo
        COMMAND cargo build --release
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ic-agent-wrapper"
        COMMENT "Compiling Rust library in ic-agent-wrapper"
        BYPRODUCTS "${IC_AGENT_WRAPPER_LIB}"
)

# Find Rust library
find_library(
        IC_AGENT_WRAPPER_LIB
        NAMES ic_agent_wrapper
        HINTS "${CMAKE_CURRENT_SOURCE_DIR}/ic-agent-wrapper/target/release"
)

# Create an imported target for the Rust library
add_library(ic_agent_wrapper STATIC IMPORTED GLOBAL)
set_target_properties(ic_agent_wrapper PROPERTIES
        IMPORTED_LOCATION "${IC_AGENT_WRAPPER_LIB}"
        )

# Make the imported target depend on cargo build
add_dependencies(ic_agent_wrapper ic_agent_wrapper_cargo)

# Compile every C file in lib-agent-c/src into a static library
file(GLOB LIB_AGENT_C_SRC "lib-agent-c/src/*.c")
add_library(agent_c STATIC ${LIB_AGENT_C_SRC})

# Compile every example in examples/
file(GLOB EXAMPLE_DIRS "examples/*")
foreach(EXAMPLE_DIR ${EXAMPLE_DIRS})
    if(IS_DIRECTORY ${EXAMPLE_DIR})
        get_filename_component(EXAMPLE_NAME ${EXAMPLE_DIR} NAME)
        file(GLOB EXAMPLE_SRC "${EXAMPLE_DIR}/*.c")
        add_executable(${EXAMPLE_NAME} ${EXAMPLE_SRC})
        add_dependencies(${EXAMPLE_NAME} ic_agent_wrapper)
        target_link_libraries(${EXAMPLE_NAME} agent_c ${IC_AGENT_WRAPPER_LIB} ${EXTRA_LIBS})
        target_include_directories(${EXAMPLE_NAME} PRIVATE "lib-agent-c/src")
    endif()
endforeach()
