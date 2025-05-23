add_executable(${PROJECT_NAME}
        Renderer.h
        Renderer.cpp
        Shape.cpp
        Shape.h
        Board.cpp
        Board.h
        GameObject.h
        Vec2.h
)
target_sources(${PROJECT_NAME} PRIVATE main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE vendor)
