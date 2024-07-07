#include "engine.h"

int main() {
    app_create(0.75, 0);

    texture_t img = texture("tiled/castle-tileset-by-RottingPixels-(CC0).png", 0);

    while (app_swap()) {
        blit(&img, 0, BLIT_RGB);
    }
}
