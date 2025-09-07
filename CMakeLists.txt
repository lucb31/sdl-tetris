cmake_minimum_required(VERSION 3.31)
project(sdl_tetris)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(src)
add_subdirectory(vendor)

target_compile_definitions(${PROJECT_NAME} PRIVATE ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets")
