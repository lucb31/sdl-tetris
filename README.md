# C++ OpenGL Summer Project (2025)

This is a personal summer project from 2025 designed to help me get hands-on experience with **C++** and **OpenGL**, focusing on building a simple framework for 2D rendering and real-time application structure.

## Learning Goals

The project was created with the following goals in mind:

- Learn and implement a real-time **game loop**
- Understand **window and input management** using SDL
- Practice modern **C++ syntax and design**
- Explore **basic 2D rendering** with OpenGL

---

## Dependencies

This project uses the following libraries:

### [SDL 3](https://github.com/libsdl-org/SDL)
- Used as a cross-platform abstraction layer for:
    - Window and input handling
    - Creating and managing OpenGL contexts

### [GLM](https://github.com/g-truc/glm)
- A header-only C++ math library for graphics software based on the OpenGL Shading Language (GLSL)
- Used for vector, matrix, and transformation math

---

## 🛠️ Build Instructions

> ⚠️ Note: This project assumes you have a C++17 (or later) compiler and CMake installed.

```bash
# Clone the repository
git clone --recursive https://github.com/lucb31/sdl-tetris.git
cd sdl-tetris 

# Create build directory
mkdir build && cd build

# Generate and build
cmake ..
make

# Run
./src/sdl_tetris
```

