cmake_minimum_required(VERSION 3.5)
project(stor)

#project version
set(VERSION 0.9.1)


#### options ###
#enable crypto (set ON either here or from command line with -DSTOR_CRYPTO=ON
#Note: setting it from here will likely require to reset (or delete) CMakeCache.txt
option(STOR_CRYPTO "Enable database encrpytion facilities" OFF)

### targets setup ###

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall ")

set(SOURCE_FILES
		include/stor/document/document.h
		include/stor/exceptions/document_exception.h
		include/stor/exceptions/store_exception.h
		include/stor/exceptions/access_exception.h
		src/document/document.cpp
		include/stor/document/iterator.h
		src/document/iterator.cpp
		include/stor/document/node.h
		src/document/node.cpp


		include/stor/store/store.h
		include/stor/store/collection.h
		include/stor/store/index_path.h
		include/stor/store/store.h
		include/stor/store/collection.h
		src/store/collection.cpp
		src/store/store.cpp
		src/store/query.cpp
		src/store/index_path.cpp
		include/stor/store/onclose.h
		src/store/onclose.cpp
		include/stor/store/query_instruction.h
		include/stor/store/eq_instruction.h
		src/store/eq_instruction.cpp
		include/stor/store/or_instruction.h
		src/store/or_instruction.cpp
		include/stor/store/access_manager.h
		src/store/access_manager.cpp
		include/stor/store/collection_visitor.h
		src/store/collection_visitor.cpp
		include/stor/document/const_iterator.h
		src/document/const_iterator.cpp
		include/stor/store/db_crypt.h
		src/store/db_crypt.cpp
		include/stor/store/and_instruction.h
		src/store/and_instruction.cpp
		src/store/gt_instruction.cpp
		include/stor/store/gte_instruction.h
		include/stor/store/gte_instruction.h
		src/store/gte_instruction.cpp
		src/store/lt_instruction.cpp
		include/stor/store/lt_instruction.h
		src/store/neq_instruction.cpp
		include/stor/store/neq_instruction.h
		src/store/lte_instruction.cpp
		include/stor/store/lte_instruction.h
		)


#platform specific configurations
if(WIN32)
	add_definitions(-DWIN32 -DTEMP_DIR="C:/Temp/")

	#dependencies injection
	if(STOR_CRYPTO)
		execute_process(
				COMMAND
				python ${stor_SOURCE_DIR}/dependencies/build.py -p mingw --with-ssl
		)
	else()
		execute_process(
				COMMAND
				python ${stor_SOURCE_DIR}/dependencies/build.py -p mingw
		)
	endif(STOR_CRYPTO)

	if (NOT EXISTS ${stor_SOURCE_DIR}/dependencies/dependencies.cmake)
		message(FATAL_ERROR "dependencies.cmake generation failed")
	endif()
	include(${stor_SOURCE_DIR}/dependencies/dependencies.cmake)

	SET(TEST_LINKS
			leveldb
			shlwapi
			pthread )

elseif(APPLE)
	add_definitions(-DUNIX -DOSX -DTEMP_DIR="/tmp/")

	#dependencies injection
	if(STOR_CRYPTO)
		execute_process(
				COMMAND
				python ${stor_SOURCE_DIR}/dependencies/build.py -p osx --with-ssl
		)
	else()
		execute_process(
				COMMAND
				python ${stor_SOURCE_DIR}/dependencies/build.py -p osx
		)
	endif(STOR_CRYPTO)
	if (NOT EXISTS ${stor_SOURCE_DIR}/dependencies/dependencies.cmake)
		message(FATAL_ERROR "dependencies.cmake generation failed")
	endif()
	include(${stor_SOURCE_DIR}/dependencies/dependencies.cmake)

	SET(TEST_LINKS
			leveldb
			pthread)

elseif (UNIX)
	add_definitions(-DUNIX -DTEMP_DIR="/tmp/")

	#dependencies injection
	if(STOR_CRYPTO)
		execute_process(
				COMMAND
				python ${stor_SOURCE_DIR}/dependencies/build.py -p linux --with-ssl
		)
	else()
		execute_process(
				COMMAND
				python ${stor_SOURCE_DIR}/dependencies/build.py -p linux
		)
	endif(STOR_CRYPTO)
	if (NOT EXISTS ${stor_SOURCE_DIR}/dependencies/dependencies.cmake)
		message(FATAL_ERROR "dependencies.cmake generation failed")
	endif()
	include(${stor_SOURCE_DIR}/dependencies/dependencies.cmake)

	SET(TEST_LINKS
			leveldb
			pthread
			dl
			)

endif()

#post-platform configurations
SET(STOR_INCLUDE_DIRS
		include
		test/include
		benchmark/include
		${catch_ROOT_DIR}/include
		${leveldb_ROOT_DIR}/include
		${rapidjson_ROOT_DIR}/include
		${boost.uuid_ROOT_DIR}/include
		)

SET(STOR_LINK_DIRS
		${leveldb_ROOT_DIR}/bin
		)

#options handling
if(STOR_CRYPTO)
	add_definitions(-DSTOR_CRYPTO)
	SET(STOR_INCLUDE_DIRS ${STOR_INCLUDE_DIRS}
			${openssl_ROOT_DIR}/include)
	SET(STOR_LINK_DIRS ${STOR_LINK_DIRS}
			${openssl_ROOT_DIR}/bin)
	SET(TEST_LINKS
			ssl
			crypto
			${TEST_LINKS})
endif(STOR_CRYPTO)

#targets

include_directories(${STOR_INCLUDE_DIRS})

link_directories(${STOR_LINK_DIRS})

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

add_library(${PROJECT_NAME}_static_pic STATIC ${SOURCE_FILES})

add_executable(${PROJECT_NAME}_test ${SOURCE_FILES}
		test/src/main.cpp
		test/src/document_test.cpp
		test/src/store_test.cpp
		test/include/stor_test/multiplatform_test_util.h
		test/include/stor_test/query_test.h
		test/src/stor_test/query_test.cpp
		test/src/access_manager_test.cpp)

add_executable(${PROJECT_NAME}_benchmark ${SOURCE_FILES}
		benchmark/src/main.cpp
		benchmark/include/ext/sqlite3.h
		benchmark/src/ext/sqlite3.c
		benchmark/src/stor_benchmark.cpp benchmark/include/stor_benchmark.h benchmark/src/sqlite_benchmark.cpp benchmark/include/sqlite_benchmark.h benchmark/include/utils.h benchmark/src/leveldb_driver.cpp benchmark/include/leveldb_driver.h)

target_link_libraries(${PROJECT_NAME}_test ${TEST_LINKS})

target_link_libraries(${PROJECT_NAME}_benchmark ${TEST_LINKS})
add_definitions(-DESFT_STOR_VERSION="${VERSION}")


set_property(TARGET ${PROJECT_NAME}_static_pic PROPERTY POSITION_INDEPENDENT_CODE ON)
