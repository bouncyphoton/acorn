#ifndef ACORN_DEBUG_GUI_H
#define ACORN_DEBUG_GUI_H

#include "game_state.h"

class DebugGui {
public:
    DebugGui();
    ~DebugGui();

    void draw();

private:
    void init();

    void destroy();
};

#endif //ACORN_DEBUG_GUI_H
