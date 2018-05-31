cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( Example )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../../.." ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

FILE(GLOB APP_SOURCES ${APP_PATH}/src/*.cpp)

add_definitions(-DCICMS_CTREE)

ci_make_app(
	SOURCES     ${APP_SOURCES}
	CINDER_PATH ${CINDER_PATH}
	BLOCKS		ciCMS
)

set ( CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ../../lib/cpp-netlib )
find_package ( cppnetlib 0.11.0 REQUIRED )
include_directories ( ${CPPNETLIB_INCLUDE_DIRS} )
target_link_libraries ( Example ${CPPNETLIB_LIBRARIES} )
