cmake_minimum_required(VERSION 3.13)
project(MyLLVMPass)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

# https://llvm.org/docs/CMake.html
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
include_directories(${LLVM_INCLUDE_DIRS})

add_subdirectory(HelloWorld)
add_subdirectory(ConstantPropagation)
add_subdirectory(DeadCodeElimination)
add_subdirectory(LoopInvariantCodeMotion)
add_subdirectory(CommonSubexpressionElimination)