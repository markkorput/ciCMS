cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( Example )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../../.." ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

FILE(GLOB APP_SOURCES ${APP_PATH}/src/*.cpp)

include(ExternalProject)

# ExternalProject_Add (
#       CPP-NETLIB
#       GIT_REPOSITORY "https://github.com/cpp-netlib/cpp-netlib"
#       GIT_TAG "0.13-release"
#       UPDATE_COMMAND ""
#       INSTALL_COMMAND "" )

ExternalProject_Add (
      CPP-NETLIB
			SOURCE_DIR "../lib/cpp-netlib")

include_directories(AFTER, ../lib/cpp-netlib)

add_definitions(-DCICMS_CTREE)

ci_make_app(
	SOURCES     ${APP_SOURCES}
	CINDER_PATH ${CINDER_PATH}
	BLOCKS		ciCMS
)
