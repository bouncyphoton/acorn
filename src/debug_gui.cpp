#include "debug_gui.h"
#include "window.h"

#include "renderer.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

bool debug_gui_init() {
    const char *glsl_version = "#version 330 core";

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window_get_glfw_window(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return true;
}

void debug_gui_shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void debug_gui_draw(GameState *game_state) {
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
        RenderStats stats = renderer_get_stats();
        ImGui::Text("Performance Stats");
        ImGui::Text("%.2fms / %.2f FPS", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Text("%d verts", stats.vertices_rendered);
        ImGui::Text("%d draw calls", stats.draw_calls);
        ImGui::Separator();

        ImGui::Text("Camera");
        ImGui::Checkbox("orbit", &game_state->camera.is_orbiting);
        ImGui::SliderFloat("fov", &game_state->camera.fov_radians, 0.0f, glm::pi<f32>());
        ImGui::DragFloat3("position", &game_state->camera.position[0], 0.1f);
        ImGui::DragFloat3("look at", &game_state->camera.look_at[0], 0.1f);
    }
    ImGui::End();

    //----------
    // rendering
    //----------

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window_get_glfw_window(), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
