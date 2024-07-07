#include "engine.h"

int main() {
    app_create(0.75, 0);

    while (app_swap()) {
        if( ui_panel("Input", UI_OPEN) ) {
            ui_section("Mouse");
            ui_mouse();
            ui_section("Keyboard");
            ui_keyboard();
            ui_section("Gamepads");
            ui_gamepads();
            ui_panel_end();
        }
    }
}
