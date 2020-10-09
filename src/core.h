#ifndef ACORN_CORE_H
#define ACORN_CORE_H

#include "config.h"
#include "game_state.h"
#include "platform.h"
#include "graphics/renderer.h"
#include "resource_manager.h"
#include "debug_gui.h"
#include <string>

class Core {
public:
    Core();
    ~Core();

    /// Run application
    [[noreturn]] void run();

    /// Quit and cleanup
    [[noreturn]] void quit();

    Config config;
    GameState gameState;
    Platform platform;
    Renderer renderer;
    ResourceManager resourceManager;
    DebugGui debugGui;
};

extern Core *core;

#endif //ACORN_CORE_H
