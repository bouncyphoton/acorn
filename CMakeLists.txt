cmake_minimum_required(VERSION 3.15)
project(acorn)

set(CMAKE_CXX_STANDARD 11)

add_executable(acorn third-party/gl3w/gl3w.c src/main.cpp src/types.h src/window.h src/window.cpp src/renderer.cpp src/renderer.h src/shader.cpp src/shader.h src/graphics_util.h src/game_state.h src/mesh.cpp src/mesh.h)

target_include_directories(acorn PUBLIC
        src/
        third-party/gl3w/include
        third-party/glm
        third-party/tinyobjloader)

# GLFW build settings
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# GLFW
add_subdirectory(third-party/glfw)
target_link_libraries(acorn glfw)

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(acorn OpenGL::GL)
