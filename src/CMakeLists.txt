add_executable(${PROJECT_NAME}
        Renderer.h
        Renderer.cpp
        Shape.cpp
        Shape.h
        Board.cpp
        Board.h
        GameObject.h
        TickTimer.cpp
        TickTimer.h
        Keymap.h
        Constants.h
        Tile.cpp
        Tile.h
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
)
target_sources(${PROJECT_NAME} PRIVATE main.cpp)

# External vendors
find_package(GLEW 2.0 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE vendor GLEW::GLEW ${OPENGL_LIBRARY})
