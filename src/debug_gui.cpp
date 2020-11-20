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

        f32 fov = core->gameState.scene.getCamera().getFov();
        glm::vec3 pos = core->gameState.scene.getCamera().getPosition();
        glm::vec2 rot = core->gameState.scene.getCamera().getLookRotation();
        f32 exposure = core->gameState.scene.getCamera().getExposure();

        ImGui::Text("Camera");
        ImGui::Text("mouse grabbed: %s (toggle with TAB)", core->platform.isMouseGrabbed() ? "true" : "false");
        ImGui::SliderFloat("fov", &fov, 0.0f, glm::pi<f32>());
        ImGui::DragFloat3("position", &pos[0], 0.1f);
        ImGui::DragFloat2("rotation", &rot[0], 0.1f);
        ImGui::SliderFloat("exposure", &exposure, 0.0f, 100.0f, "%.3f", 2);

        core->gameState.scene.getCamera().setFov(fov);
        core->gameState.scene.getCamera().setPosition(pos);
        core->gameState.scene.getCamera().setLookRotation(rot);
        core->gameState.scene.getCamera().setExposure(exposure);

        if (ImGui::Button("reload shaders")) {
            core->renderer.reloadShaders();
        }
    }
    ImGui::End();

    //----------
    // rendering
    //----------

    const ConfigData &config = core->config.getConfigData();

    ImGui::Render();
    glViewport(0, 0, config.width, config.height);
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
