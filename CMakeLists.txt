cmake_minimum_required(VERSION 3.5)
project(ColorQuant)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(Boost REQUIRED COMPONENTS iostreams filesystem system)

include_directories(
	${ColorQuant_SOURCE_DIR}/include
	${PNG_INCLUDE_DIRS}
	${JPEG_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

file(GLOB_RECURSE sources_cq ${ColorQuant_SOURCE_DIR}/src/*.cpp)

add_executable(ColorQuant ${sources_cq})

target_link_libraries(ColorQuant
	${PNG_LIBRARIES}
	${JPEG_LIBRARIES}
	${Boost_LIBRARIES}
)
