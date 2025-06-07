add_executable(${PROJECT_NAME}
        Renderer.h
        Renderer.cpp
        Shape.cpp
        Shape.h
        Board.cpp
        Board.h
        GameObject.h
        Vec2.h
        TickTimer.cpp
        TickTimer.h
        Keymap.h
        Constants.h
        Tile.cpp
        Tile.h
        Mat3.h
        Vec3.h
        Benchmark/Instrumentor.h
)
target_sources(${PROJECT_NAME} PRIVATE main.cpp)

# External vendors
find_package(GLEW 2.0 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE vendor GLEW::GLEW ${OPENGL_LIBRARY})
