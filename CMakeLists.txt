cmake_minimum_required(VERSION 3.31)
project(sdl_tetris)

set(CMAKE_CXX_STANDARD 20)
# Required to copy assets during build step
set(ASSETS_SRC_DIR "${CMAKE_SOURCE_DIR}/assets")
set(ASSETS_DEST_DIR "${CMAKE_BINARY_DIR}/assets")

add_subdirectory(src)
add_subdirectory(vendor)

# Define compiler definitions for asset paths
target_compile_definitions(${PROJECT_NAME} PRIVATE FONTS_DIR="./assets/fonts/")
target_compile_definitions(${PROJECT_NAME} PRIVATE SHADERS_DIR="./assets/shaders/")
