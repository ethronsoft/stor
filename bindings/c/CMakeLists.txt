cmake_minimum_required(VERSION 3.5)
project(cstor)

#project version
set(VERSION 0.0.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall ")

#stor dependency

#### STOR options ###
#enable crypto (set ON either here or from command line with -DSTOR_CRYPTO=ON
#Note: setting it from here will likely require to reset (or delete) CMakeCache.txt
option(STOR_CRYPTO "Enable database encrpytion facilities" OFF)
add_subdirectory(../../core ${CMAKE_BINARY_DIR}/deps)

#dependencies info
include(${stor_SOURCE_DIR}/dependencies/dependencies.cmake )

#sources
set(SOURCE_FILES
        include/cstor/internal/opaque_types.h
        include/cstor/document/cdocument.h
        include/cstor/document/cnode.h
        src/cstor/document/cdocument.cpp
        src/cstor/errors/errors.c
        src/cstor/document/cnode.cpp
        include/cstor/store/cstore.h include/cstor/store/ccollection.h)

#directories
SET(CSTOR_INCLUDE_DIRS
        include
        test/include
        ../../core/include
        ${catch_ROOT_DIR}/include
        ${leveldb_ROOT_DIR}/include
        ${rapidjson_ROOT_DIR}/include
        ${boost.uuid_ROOT_DIR}/include
        )

SET(CSTOR_LINK_DIRS
        ${leveldb_ROOT_DIR}/bin
        )

SET(CSTOR_TEST_LIBRARIES
        stor
        leveldb
        pthread )

#platform specific configurations
if(WIN32)
    SET(CSTOR_TEST_LIBRARIES ${CSTOR_TEST_LIBRARIES}
            shlwapi)
elseif(APPLE)
    add_definitions(-DUNIX -DOSX)

elseif(UNIX)
    add_definitions(-DUNIX)
    SET(CSTOR_TEST_LIBRARIES ${CSTOR_TEST_LIBRARIES}
            dl)
endif()

# optional directories
if(STOR_CRYPTO)
    add_definitions(-DSTOR_CRYPTO)
    SET(CSTOR_INCLUDE_DIRS ${CSTOR_INCLUDE_DIRS}
            ${openssl_ROOT_DIR}/include)
    SET(CSTOR_LINK_DIRS ${CSTOR_LINK_DIRS}
            ${openssl_ROOT_DIR}/bin)
    SET(CSTOR_TEST_LIBRARIES ${CSTOR_TEST_LIBRARIES}
            ssl
            crypto)
endif(STOR_CRYPTO)

# targets
include_directories(${CSTOR_INCLUDE_DIRS})
link_directories(${CSTOR_LINK_DIRS})


add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} include/cstor/errors/errors.h)

#test
add_executable(${PROJECT_NAME}_test ${SOURCE_FILES}
        test/main.cpp
        test/document_test.cpp)

target_link_libraries(${PROJECT_NAME}_test ${CSTOR_TEST_LIBRARIES})
