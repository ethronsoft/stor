cmake_minimum_required(VERSION 3.5)
project(pystor)

#project version
set(VERSION 0.9.0)

option(STOR_CRYPTO "Enable database encrpytion facilities" OFF)
set( CMAKE_BUILD_TYPE "Release" )
add_subdirectory(../c ${CMAKE_BINARY_DIR}/pydeps)

add_custom_target(${PROJECT_NAME}_setup
        DEPENDS cstor)

if(WIN32)
    add_custom_command(
            TARGET ${PROJECT_NAME}_setup POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_BINARY_DIR}/pydeps/libcstor.dll
            ${CMAKE_SOURCE_DIR}/ethronsoft/pystor/bin/libcstor.dll)
elseif(APPLE)
    add_custom_command(
            TARGET ${PROJECT_NAME}_setup POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_BINARY_DIR}/pydeps/libcstor.dylib
            ${CMAKE_SOURCE_DIR}/ethronsoft/pystor/bin/libcstor.dylib)
else()
    add_custom_command(
            TARGET ${PROJECT_NAME}_setup POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_BINARY_DIR}/pydeps/libcstor.so
            ${CMAKE_SOURCE_DIR}/ethronsoft/pystor/bin/libcstor.so)
endif()


