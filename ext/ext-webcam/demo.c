#include "engine.h"

int main() {
    app_create(0.75, 0);

    while(app_swap()) {
        texture_t *t = webcam(1);
        if(t) blit(t, 0, BLIT_Y);
    }
}
