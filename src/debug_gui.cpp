#include "debug_gui.h"
#include "core.h"
#include "renderer.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

void DebugGui::init() {
    const char *glsl_version = "#version 330 core";

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(core->platform.getGlfwWindow(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void DebugGui::destroy() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void DebugGui::draw() {
    //----------
    // new frame
    //----------

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //----------
    // draw gui
    //----------

    ImGuiIO &io = ImGui::GetIO();

    ImGui::Begin("Debug");
    {
        RenderStats stats = core->renderer.getStats();
        ImGui::Text("Performance Stats");
        ImGui::Text("%.2fms / %.2f FPS", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Text("%d verts", stats.vertices_rendered);
        ImGui::Text("%d draw calls", stats.draw_calls);
        ImGui::Separator();

        ImGui::Text("Camera");
        ImGui::Checkbox("orbit", &core->game_state.camera.is_orbiting);
        ImGui::SliderFloat("fov", &core->game_state.camera.fov_radians, 0.0f, glm::pi<f32>());
        ImGui::DragFloat3("position", &core->game_state.camera.position[0], 0.1f);
        ImGui::DragFloat3("look at", &core->game_state.camera.look_at[0], 0.1f);
    }
    ImGui::End();

    //----------
    // rendering
    //----------

    ImGui::Render();
    glViewport(0, 0, core->game_state.render_options.width, core->game_state.render_options.height);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
