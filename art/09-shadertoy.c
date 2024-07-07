// shadertoy viewer
// - rlyeh, public domain

#include "engine.h"

int main() {
    app_create(75, 0);

    int count;
    char **list = file_list("shadertoys/**.fs", &count);

    shadertoy_t sh = shadertoy(*list, 0); // 0:no flags

    while(app_swap()) {
        // selector
        static int selected = 0;
        int prev = input_down(KEY_UP) || input_down(KEY_LEFT);
        int next = input_down(KEY_DOWN) || input_down(KEY_RIGHT);
        if( prev ) if( selected > 0 ) sh = shadertoy( list[--selected], 0 );
        if( next ) if( selected < array_count(list) - 1 ) sh = shadertoy( list[++selected], 0 );

        // draw
        shadertoy_render(&sh, window_delta());

        // UI
        if( ui_panel("Shadertoy", UI_OPEN)) {
            for( int i = 0; i < count; ++i ) {
                bool in_use = i == selected;
                if( ui_bool(file_name(list[i]), &in_use) ) {
                    sh = shadertoy( list[selected = i], 0 );
                }
            }
            ui_panel_end();
        }
    }
}
