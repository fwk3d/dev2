#include "engine.h"

int main() {
    // 80% sized, MSAA x4 flag
    app_create(80.0, APP_MSAA4);

    // tiled map
    tiled_t tmx = tiled(file_read("castle.tmx", 0));
    // tmx.parallax = true;

    // camera 2d
    camera_t cam = camera();
    cam.position = vec3(window_width()/2, window_height()/2, 3); // at(CX,CY) zoom(x3)
    camera_enable(&cam);

    // fx: load all post fx files in all subdirs. enable a few filters by default
    fx_load("fx**.fs");
    fx_enable(fx_find("fxCRT2.fs"), 1);
    fx_enable(fx_find("fxGrain.fs"), 1);
    fx_enable(fx_find("fxContrast.fs"), 1);
    fx_enable(fx_find("fxVignette.fs"), 1);

    // sort them
    fx_order(fx_find("fxCRT2.fs"), 0);
    fx_order(fx_find("fxGrain.fs"), 1);
    fx_order(fx_find("fxContrast.fs"), 2);
    fx_order(fx_find("fxVignette.fs"), 3);

    // demo loop
    while (app_swap() && !input_down(KEY_ESC)) {

        // camera panning (x,y) & zooming (z)
        if( !ui_hovered() && !ui_active() ) {
            if( input(MOUSE_L) ) cam.position.x += input_diff(MOUSE_X);
            if( input(MOUSE_L) ) cam.position.y += input_diff(MOUSE_Y);
            cam.position.z += input_diff(MOUSE_W) * 0.1;
        }

        // apply post-fxs from here
        fx_begin();

            profile("Rendering") {
                vec3 center = add3(cam.position,vec3(-window_width()/1,-window_height()/2,0));
                // render tiled map
                tiled_render(tmx, center);
            }

        // post-fxs end here
        fx_end(0,0);

        if( ui_panel("Tiled", UI_OPEN)) {
            ui_float("Zoom in", &cam.position.z);
            ui_tiled(&tmx);
            ui_panel_end();
        }
    }
}
