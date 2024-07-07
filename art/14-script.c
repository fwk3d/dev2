#include "engine.h"

API void hello_from_C() {
    puts("hello from C");
}

int main() {
    app_create(0.75, 0);

    lua_init();
    luaj_init();
    luaj_bind("API void hello_from_C();");

    while( app_swap() ) {
        if( ui_panel("Script", UI_OPEN) ) {
            if( ui_button("push me") ) {
                lua_runstring("c.hello_from_C()");
                lua_runstring("c.ui_notify(_VERSION, 'Hello from Lua!')");
            }
            ui_panel_end();
        }
    }
}
