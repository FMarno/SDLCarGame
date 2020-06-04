cmake_minimum_required(VERSION 3.10)

project(SDLTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)

find_package(SDL2IMAGE REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})

add_executable(test main.cpp)
target_link_libraries(test ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})
