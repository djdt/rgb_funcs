cmake_minimum_required(VERSION 3.5)
project(RGBFuncs)

set(CMAKE_BUILD_TYPE Release)

find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)

include_directories(
	${RGBFuncs_SOURCE_DIR}/include
	${PNG_INCLUDE_DIRS}
	${JPEG_INCLUDE_DIRS}
)

file(GLOB_RECURSE sources_rgb ${RGBFuncs_SOURCE_DIR}/src/*.cpp)

add_library(RGBFuncs ${sources_rgb})

target_link_libraries(RGBFuncs
	${PNG_LIBRARIES}
	${JPEG_LIBRARIES}
)
