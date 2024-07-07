#include "engine.h"

int main() {
    app_create(0.75, 0);

    camera_t cam = camera();

    fx_load("**fxVCR.fs");
    fx_enable(fx_find("fxVCR.fs"), 1);

    while (app_swap()) {
        // fps camera
        camera_freefly(&cam, 0);

        fx_begin();

            ddraw_grid(0);
            ddraw_demo();
            ddraw_flush();

        fx_end(0,0);
    }
}
