
cmake_minimum_required(VERSION 3.6)
project(stor)
set(VERSION 0.0.1)

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
		include/stor/store/lte_instruction.h)

		
if(WIN32)
	add_definitions(-DWIN32)

	#dependencies injection
	execute_process(
			COMMAND
				python ${CMAKE_SOURCE_DIR}/dependencies/build.py -p mingw
			)
	if (NOT EXISTS ${CMAKE_SOURCE_DIR}/dependencies/dependencies.cmake)
		message(FATAL_ERROR "dependencies.cmake generation failed")
	endif()
	include(${CMAKE_SOURCE_DIR}/dependencies/dependencies.cmake)
	
	
	include_directories(
			include
			test/include
			${catch_ROOT_DIR}/include
			${leveldb_ROOT_DIR}/include
			${rapidjson_ROOT_DIR}/include
			${boost.uuid_ROOT_DIR}/include
	)
	
	add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} )

		link_directories(
				${leveldb_ROOT_DIR}/bin
		)

	add_executable(${PROJECT_NAME}_test ${SOURCE_FILES}
			test/src/main.cpp
			test/src/document_test.cpp
			test/src/store_test.cpp
			test/include/stor_test/multiplatform_test_util.h
			test/include/stor_test/query_test.h
			test/src/stor_test/query_test.cpp
	)
	
	target_link_libraries(${PROJECT_NAME}_test leveldb shlwapi pthread)
	
elseif(APPLE)
	add_definitions(-DUNIX -DOSX)

	#dependencies injection
	execute_process(
			COMMAND
			python ${CMAKE_SOURCE_DIR}/dependencies/build.py -p osx
	)
	if (NOT EXISTS ${CMAKE_SOURCE_DIR}/dependencies/dependencies.cmake)
		message(FATAL_ERROR "dependencies.cmake generation failed")
	endif()
	include(${CMAKE_SOURCE_DIR}/dependencies/dependencies.cmake)


	include_directories(
			include
			test/include
			${catch_ROOT_DIR}/include
			${leveldb_ROOT_DIR}/include
			${rapidjson_ROOT_DIR}/include
			${boost.uuid_ROOT_DIR}/include
	)

	add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} )

	link_directories(
			${leveldb_ROOT_DIR}/bin
	)

	add_executable(${PROJECT_NAME}_test ${SOURCE_FILES}
			test/src/main.cpp
			test/src/document_test.cpp
			test/src/store_test.cpp
			test/include/stor_test/multiplatform_test_util.h
			test/include/stor_test/query_test.h
			test/src/stor_test/query_test.cpp
			)
	
	target_link_libraries(${PROJECT_NAME}_test leveldb pthread)
	
elseif (UNIX)
	add_definitions(-DUNIX)

	#dependencies injection
	execute_process(
			COMMAND
			python ${CMAKE_SOURCE_DIR}/dependencies/build.py -p linux
	)
	if (NOT EXISTS ${CMAKE_SOURCE_DIR}/dependencies/dependencies.cmake)
		message(FATAL_ERROR "dependencies.cmake generation failed")
	endif()
	include(${CMAKE_SOURCE_DIR}/dependencies/dependencies.cmake)


	include_directories(
			include
			test/include
			${catch_ROOT_DIR}/include
			${leveldb_ROOT_DIR}/include
			${rapidjson_ROOT_DIR}/include
			${boost.uuid_ROOT_DIR}/include
	)

	add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} )

	link_directories(
			${leveldb_ROOT_DIR}/bin
	)

	add_executable(${PROJECT_NAME}_test ${SOURCE_FILES}
			test/src/main.cpp
			test/src/document_test.cpp
			test/src/store_test.cpp
			test/include/stor_test/multiplatform_test_util.h
			test/include/stor_test/query_test.h
			test/src/stor_test/query_test.cpp
			)
	
	target_link_libraries(${PROJECT_NAME}_test leveldb pthread)
	
endif()	
