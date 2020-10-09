cmake_minimum_required(VERSION 3.15)
project(acorn)

set(CMAKE_CXX_STANDARD 14)

add_executable(acorn
        third-party/gl3w/gl3w.c third-party/imgui/imgui.cpp third-party/imgui/imgui_demo.cpp third-party/imgui/imgui_draw.cpp third-party/imgui/imgui_impl_glfw.cpp third-party/imgui/imgui_impl_opengl3.cpp third-party/imgui/imgui_widgets.cpp
        src/main.cpp src/types.h src/graphics/renderer.cpp src/graphics/renderer.h src/graphics/shader.cpp src/graphics/shader.h src/game_state.h src/graphics/model.cpp src/graphics/model.h src/graphics/material.h src/transform.h src/graphics/texture.cpp src/graphics/texture.h src/utils.h src/utils.cpp src/framebuffer.cpp src/framebuffer.h src/debug_gui.cpp src/debug_gui.h src/core.cpp src/core.h src/platform.cpp src/platform.h src/constants.h src/resource_manager.cpp src/resource_manager.h src/graphics/vertex.h src/graphics/mesh.h src/scene.cpp src/scene.h src/graphics/mesh.cpp src/entity.h src/config.cpp src/config.h src/graphics/render_context.cpp src/graphics/render_context.h src/log.h src/camera.cpp src/camera.h)

target_include_directories(acorn PUBLIC
        src/
        third-party/gl3w/include
        third-party/glm
        third-party/tinyobjloader
        third-party/stb
        third-party/imgui)

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
