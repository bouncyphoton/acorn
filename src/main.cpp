#include "window.h"
#include "renderer.h"
#include <cstdio>

static bool acorn_init() {
    u32 window_width = 800;
    u32 window_height = 600;
    const char *window_title = "acorn";

    if (!window_init(window_width, window_height, window_title)) return false;
    if (!renderer_init()) return false;

    puts("[info] acorn successfully initialized");

    return true;
}

static void acorn_shutdown() {
    puts("[info] shutting down acorn");

    renderer_shutdown();
    window_shutdown();
}

static void acorn_run() {
    while (!window_should_close()) {
        window_update();

        renderer_draw();

        window_swap_buffers();
    }
}

int main() {
    // run if initialized successfully
    if (acorn_init()) {
        acorn_run();
    }

    // shutdown
    acorn_shutdown();

    return 0;
}
