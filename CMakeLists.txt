cmake_minimum_required(VERSION 3.5)
project(rgb_funcs)

set(CMAKE_BUILD_TYPE Release)

find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)

include_directories(
	${${PROJECT_NAME}_SOURCE_DIR}/include
	${PNG_INCLUDE_DIRS}
	${JPEG_INCLUDE_DIRS}
)

file(GLOB_RECURSE SRC_FILES ${${PROJECT_NAME}_SOURCE_DIR}/src/*.cpp)

add_library(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME}
	${PNG_LIBRARIES}
	${JPEG_LIBRARIES}
)
