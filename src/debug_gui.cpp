#include "debug_gui.h"
#include "core.h"
#include "log.h"
#include "graphics/renderer.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

DebugGui::DebugGui() {
    Log::debug("DebugGui::DebugGui()");
    init();
}

DebugGui::~DebugGui() {
    Log::debug("DebugGui::~DebugGui()");
    destroy();
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
        ImGui::Text("%d verts", stats.verticesRendered);
        ImGui::Text("%d draw calls", stats.drawCalls);
        ImGui::Separator();

        f32 fov = core->gameState.camera.getFov();
        glm::vec3 pos = core->gameState.camera.getPosition();
        glm::vec3 lookPos = core->gameState.camera.getLookPosition();
        f32 exposure = core->gameState.camera.getExposure();

        ImGui::Text("Camera");
        ImGui::SliderFloat("fov", &fov, 0.0f, glm::pi<f32>());
        ImGui::DragFloat3("position", &pos[0], 0.1f);
        ImGui::DragFloat3("look at", &lookPos[0], 0.1f);
        ImGui::SliderFloat("exposure", &exposure, 0.0f, 100.0f, "%.3f", 2);

        core->gameState.camera.setFov(fov);
        core->gameState.camera.setPosition(pos);
        core->gameState.camera.setLookPosition(lookPos);
        core->gameState.camera.setExposure(exposure);

        if (ImGui::Button("reload shaders")) {
            core->renderer.reloadShaders();
        }
    }
    ImGui::End();

    //----------
    // rendering
    //----------

    ImGui::Render();
    glViewport(0, 0, core->gameState.renderOptions.width, core->gameState.renderOptions.height);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugGui::init() {
    const char *glslVersion = "#version 330 core";

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(core->platform.getGlfwWindow(), true);
    ImGui_ImplOpenGL3_Init(glslVersion);
}

void DebugGui::destroy() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
