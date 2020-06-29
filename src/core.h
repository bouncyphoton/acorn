#ifndef ACORN_CORE_H
#define ACORN_CORE_H

#include "game_state.h"
#include "platform.h"
#include "renderer.h"
#include "resource_manager.h"
#include "debug_gui.h"
#include <string>

class Core {
public:
    /// Run application
    [[noreturn]] void run();

    /// Quit and cleanup
    [[noreturn]] void quit();

    /// Log an informational message
    void info(const std::string &msg);

    /// Log a warning
    void warn(const std::string &msg);

    /// Log a fatal error and quit
    [[noreturn]] void fatal(const std::string &msg);

    GameState game_state;
    Platform platform;
    Renderer renderer;
    ResourceManager resourceManager;
    DebugGui debugGui;

private:
    void init();

    void cleanup();
};

extern Core *core;

#endif //ACORN_CORE_H
