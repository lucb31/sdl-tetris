add_executable(${PROJECT_NAME}
        Renderer.h
        Renderer.cpp)
target_sources(${PROJECT_NAME} PRIVATE main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE vendor)
