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

    /// Log an informational message
    void info(const std::string &msg);

    /// Log a warning
    void warn(const std::string &msg);

    /// Log a debug message
    void debug(const std::string &msg);

    /// Log a fatal error and quit
    [[noreturn]] void fatal(const std::string &msg);

    Config config;
    GameState gameState;
    Platform platform;
    Renderer renderer;
    ResourceManager resourceManager;
    DebugGui debugGui;
};

extern Core *core;

#endif //ACORN_CORE_H
