cmake_minimum_required(VERSION 2.8)
project(Df3D)

set(CMAKE_CXX_FLAGS "-g -Wall")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file(
	GLOB
	SRCS
	src/*.cpp
)

find_package(PkgConfig REQUIRED)

pkg_check_modules(OGRE OGRE REQUIRED)
pkg_check_modules(OIS OIS REQUIRED)

include_directories(${OGRE_INCLUDE_DIRS})
include_directories(${OIS_INCLUDE_DIRS})

set( mainLibs ${OGRE_LIBRARIES} ${OIS_LIBRARIES} )

add_executable(Df3D ${SRCS})
target_link_libraries(Df3D ${mainLibs})

configure_file(bin/resources.cfg resources.cfg COPYONLY)
configure_file(bin/plugins.cfg plugins.cfg COPYONLY)

file(COPY res DESTINATION .)
file(COPY bin/ DESTINATION .)
