cmake_minimum_required(VERSION 3.10)
project(ICP-C)

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

if(UNIX AND NOT APPLE)
    set(EXTRA_LIBS m)
endif()

# Set Rust library path
set(IC_AGENT_WRAPPER_LIB "${CMAKE_CURRENT_SOURCE_DIR}/ic-agent-wrapper/target/release/libic_agent_wrapper.a")
set(BINDING_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ic-agent-wrapper/bindings.h")
set(OUT_BINDING_FILE "${CMAKE_CURRENT_SOURCE_DIR}/lib-agent-c/inc/zondax_ic.h")

# Compile Rust library in ic-agent-wrapper using cargo build
add_custom_command(
        OUTPUT ${IC_AGENT_WRAPPER_LIB}
        COMMAND cargo build --release
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ic-agent-wrapper"
        COMMENT "Compiling Rust library in ic-agent-wrapper"
)

add_custom_target(ic_agent_wrapper_build ALL
        DEPENDS ${IC_AGENT_WRAPPER_LIB}
        )

# Create an imported target for the Rust library
add_library(ic_agent_wrapper STATIC IMPORTED GLOBAL)
set_target_properties(ic_agent_wrapper PROPERTIES
        IMPORTED_LOCATION "${IC_AGENT_WRAPPER_LIB}"
        )

# Make the imported target depend on the Rust library build
add_dependencies(ic_agent_wrapper ic_agent_wrapper_build)

# Compile every C file in lib-agent-c/src into a static library
include_directories("lib-agent-c/inc")
file(GLOB LIB_AGENT_C_SRC "lib-agent-c/src/*.c")
add_library(agent_c STATIC ${LIB_AGENT_C_SRC})
configure_file(${BINDING_FILE} ${OUT_BINDING_FILE} COPYONLY)

# Compile every C++ file in lib-agent-cpp/src into a static library
include_directories("lib-agent-cpp/inc")
# Using file(GLOB) causes linkage errors as it includes the tests.cpp file 
# leading to duplicated symbols. So now we need to add any other new source here 
# manually.
# file(GLOB LIB_AGENT_CPP_SRC "lib-agent-cpp/src/*.cpp")
set(LIB_AGENT_CPP_SRC
    lib-agent-cpp/src/agent.cpp
    lib-agent-cpp/src/func.cpp
    lib-agent-cpp/src/helper.cpp
    lib-agent-cpp/src/identity.cpp
    lib-agent-cpp/src/identity.cpp
    lib-agent-cpp/src/idl_args.cpp
    lib-agent-cpp/src/idl_value.cpp
    lib-agent-cpp/src/principal.cpp
    lib-agent-cpp/src/service.cpp
)
add_library(agent_cpp STATIC ${LIB_AGENT_CPP_SRC})
target_link_libraries(agent_cpp ic_agent_wrapper agent_c)
# We do not want to include in the final binary the testing machinery
target_compile_definitions(agent_cpp PRIVATE DOCTEST_CONFIG_DISABLE)

# Artifact for testing, this makes possible to run unit-test using 
# only one test target
add_library(agent_cpp_tests OBJECT ${LIB_AGENT_CPP_SRC})
target_link_libraries(agent_cpp_tests ic_agent_wrapper agent_c)

add_custom_target(tests)
add_executable(test "lib-agent-cpp/src/tests.cpp")
target_link_libraries(test agent_cpp_tests ic_agent_wrapper agent_c)
target_compile_features(test PRIVATE cxx_std_17)
add_dependencies(tests test)

# Compile every example in examples/
file(GLOB EXAMPLE_DIRS "examples/*")
foreach(EXAMPLE_DIR ${EXAMPLE_DIRS})
    if(IS_DIRECTORY ${EXAMPLE_DIR})
        get_filename_component(EXAMPLE_NAME ${EXAMPLE_DIR} NAME)
        file(GLOB EXAMPLE_SRC "${EXAMPLE_DIR}/*.c")
        add_executable(${EXAMPLE_NAME} ${EXAMPLE_SRC})
        add_dependencies(${EXAMPLE_NAME} ic_agent_wrapper)
        target_link_libraries(${EXAMPLE_NAME} agent_c ${IC_AGENT_WRAPPER_LIB} ${EXTRA_LIBS})
        target_include_directories(${EXAMPLE_NAME} PRIVATE "lib-agent-c/inc")
    endif()
endforeach()

add_custom_target(examples)
file(GLOB EXAMPLE_DIRS "examples-cpp/*")
foreach(EXAMPLE_DIR ${EXAMPLE_DIRS})
    if(IS_DIRECTORY ${EXAMPLE_DIR})
        get_filename_component(EXAMPLE_NAME ${EXAMPLE_DIR} NAME)
        file(GLOB EXAMPLE_SRC "${EXAMPLE_DIR}/*.cpp")
        add_executable(${EXAMPLE_NAME} ${EXAMPLE_SRC})
        add_dependencies(${EXAMPLE_NAME} ic_agent_wrapper agent_c)
        add_dependencies(examples ${EXAMPLE_NAME})
        target_link_libraries(${EXAMPLE_NAME} agent_cpp ${EXTRA_LIBS})
        target_include_directories(${EXAMPLE_NAME} PRIVATE "lib-agent-cpp/inc")
    endif()
endforeach()
