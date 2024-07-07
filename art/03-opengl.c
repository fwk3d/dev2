#include "engine.h"

int main() {
    vec4 color = vec4(0.0,0.4,0.5,0.500);

    app_create(0.75, APP_TRANSPARENT);
    while( app_swap() ) {
        glClearColor(color.x,color.y,color.z,color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        if( ui_color4f("ClearColor", &color.x) );
    }
}
