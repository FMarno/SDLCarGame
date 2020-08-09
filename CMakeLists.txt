cmake_minimum_required(VERSION 3.15)

project(SDLTest)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRC_FILES
    src/main.cpp
    src/helpers.cpp
    src/SpriteSheet.cpp
    src/Character.cpp
)


find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)

add_executable(test WIN32 ${SRC_FILES})
target_link_libraries(test SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_image)
