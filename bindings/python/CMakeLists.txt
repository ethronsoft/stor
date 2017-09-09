cmake_minimum_required(VERSION 3.5)
project(pystor)

#project version
set(VERSION 0.0.1)

add_subdirectory(../c ${CMAKE_BINARY_DIR}/pydeps)

add_custom_target(${PROJECT_NAME}_setup
        DEPENDS cstor)

add_custom_command(
        TARGET ${PROJECT_NAME}_setup POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/pydeps/libcstor.dll
        ${CMAKE_SOURCE_DIR}/deps/libcstor.dll)

