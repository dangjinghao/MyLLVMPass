set(PASS_NAME "HelloWorld")


set(PASS_NAME_EXT "${PASS_NAME}Pass")

add_library(${PASS_NAME_EXT} MODULE Pass.cpp)

target_compile_definitions(${PASS_NAME_EXT} PRIVATE PASS_NAME="${PASS_NAME}")
target_compile_definitions(${PASS_NAME_EXT} PRIVATE PASS_NAME_EXT="${PASS_NAME_EXT}")

set_target_properties(${PASS_NAME_EXT} PROPERTIES PREFIX "")
message(STATUS "Pass ${PASS_NAME} loaded")