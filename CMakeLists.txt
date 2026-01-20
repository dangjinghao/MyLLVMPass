cmake_minimum_required(VERSION 3.13)
project(HelloWorldLLVMPassPlugin CXX)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

# Pass 
add_library(HelloWorld SHARED HelloWorld.cpp)
set_target_properties(HelloWorld PROPERTIES PREFIX "")