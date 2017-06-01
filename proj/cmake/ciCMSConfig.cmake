if( NOT TARGET ciCMS )
	get_filename_component( ciCMS_SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../src" ABSOLUTE )
	get_filename_component( CINDER_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE )

	FILE(GLOB ciCMS_SOURCES ${ciCMS_SOURCE_PATH}/ciCMS/*.cpp)

	add_library( ciCMS ${ciCMS_SOURCES} )

	target_include_directories( ciCMS PUBLIC "${ciCMS_SOURCE_PATH}" )
	target_include_directories( ciCMS SYSTEM BEFORE PUBLIC "${CINDER_PATH}/include" )

	if( NOT TARGET cinder )
		    include( "${CINDER_PATH}/proj/cmake/configure.cmake" )
		    find_package( cinder REQUIRED PATHS
		        "${CINDER_PATH}/${CINDER_LIB_DIRECTORY}"
		        "$ENV{CINDER_PATH}/${CINDER_LIB_DIRECTORY}" )
	endif()
	target_link_libraries( ciCMS PRIVATE cinder )
endif()
