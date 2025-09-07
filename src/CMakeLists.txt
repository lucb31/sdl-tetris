add_executable(${PROJECT_NAME}
        Renderer.h
        Renderer.cpp
        Tetris/Shape.cpp
        Tetris/Shape.h
        Tetris/Board.cpp
        Tetris/Board.h
        GameObject.h
        TickTimer.cpp
        TickTimer.h
        Tetris/Keymap.h
        Tetris/Constants.h
        Tetris/Tile.cpp
        Tetris/Tile.h
        Benchmark/Instrumentor.h
        Texture.cpp
        Texture.h
        stb_image.h
        StringRenderer.cpp
        StringRenderer.h
        Shader.cpp
        Shader.h
        FontAsset.cpp
        FontAsset.h
        Collision.h
        Tetris/ShapeConfiguration.h
        Tetris/ShapeQueue.cpp
        Tetris/ShapeQueue.h
)
target_sources(${PROJECT_NAME} PRIVATE main.cpp)
# Make sure binary ends up in build root
set_target_properties(sdl_tetris PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# External vendors
find_package(GLEW 2.0 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE vendor GLEW::GLEW ${OPENGL_LIBRARY})

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ASSETS_SRC_DIR}" "${ASSETS_DEST_DIR}"
        COMMENT "Copying assets to build output."
)
