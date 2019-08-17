if( NOT TARGET ciCMS )
	get_filename_component( ciCMS_SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../src" ABSOLUTE )
	get_filename_component( CINDER_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE )
	get_filename_component( lib_PATH "${CMAKE_CURRENT_LIST_DIR}/../../lib" ABSOLUTE )

	FILE(GLOB ciCMS_SOURCES ${ciCMS_SOURCE_PATH}/ciCMS/*.cpp ${ciCMS_SOURCE_PATH}/ciCMS/**/*.cpp ${ciCMS_SOURCE_PATH}/ciCMS/**/**/*.cpp ${ciCMS_SOURCE_PATH}/ciCMS/**/**/**/*.cpp ${lib_PATH}/ctree/src/**/*.cpp ${lib_PATH}/info/src/**/*.cpp)

	add_library( ciCMS ${ciCMS_SOURCES} )

	target_include_directories( ciCMS PUBLIC "${ciCMS_SOURCE_PATH}" "${lib_PATH}/ctree/include")
	target_include_directories( ciCMS PUBLIC "${ciCMS_SOURCE_PATH}" "${lib_PATH}/info/include")
	target_include_directories( ciCMS SYSTEM BEFORE PUBLIC "${CINDER_PATH}/include" )

	if( NOT TARGET cinder )
		    include( "${CINDER_PATH}/proj/cmake/configure.cmake" )
		    find_package( cinder REQUIRED PATHS
		        "${CINDER_PATH}/${CINDER_LIB_DIRECTORY}"
		        "$ENV{CINDER_PATH}/${CINDER_LIB_DIRECTORY}" )
	endif()
	target_link_libraries( ciCMS PRIVATE cinder )
endif()
