#include "engine.h"

int main() {
    app_create(0.75, 0);

    audio_t snd = audio( "the_entertainer.ogg", 0, 0 );

    speaker_t spk = speaker();
    speaker_play( &spk, snd, 0 );

    camera_t cam = camera(); {
        cam.position = (vec3){0, 2, 0};
        cam.yaw = 90;
        cam.pitch = 0;
    }

    vec3 sound_position = (vec3){0, 2.5, 3};

    while( app_swap() && !input(KEY_ESC) ) {
        camera_freefly(&cam, false);
        listener_position(&cam.position.x);
        listener_velocity(&cam.velocity.x);
        listener_direction(&cam.lookdir.x);

        ddraw_grid(0);

        sound_position.x = sin(time_ss()) * 3;
        ddraw_sphere(sound_position, 0.5);
        speaker_position(&spk, &sound_position.x, 0);
    }
}
