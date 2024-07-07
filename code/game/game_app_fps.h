//-----------------------------------------------------------------------------
// fps locking

API int limit_fps(float fps);

#if CODE

extern volatile float target_fps;
static volatile unsigned limit_fps_running, timer_counter, loop_counter;
static
int limit_fps_thread(void *arg) {
    int64_t ns_excess = 0;
    while( limit_fps_running ) {
        if( target_fps <= 1 ) {
            loop_counter = timer_counter = 0;
            SDL_Delay(250);
        } else {
            timer_counter++;
            int64_t tt = (int64_t)(1e9/(float)target_fps) - ns_excess;
            uint64_t took = -SDL_GetTicksNS();
            SDL_DelayPrecise( tt > 0 ? (float)tt : 0.f );
            took += SDL_GetTicksNS();
            ns_excess = took - tt;
            if( ns_excess < 0 ) ns_excess = 0;
            //puts( strf("%lld", ns_excess) );
        }
    }
    limit_fps_running = 1;

    (void)arg;
    return 0;
}

//-----------------------------------------------------------------------------

// function that locks render to desired `FPS` framerate.
// - returns true if must render, else 0.

int limit_fps(float fps) {
    target_fps = fps * (fps > 0);
    if( target_fps <= 0 ) return 1;

    do_once {
        // private threaded timer
        limit_fps_running = 1, timer_counter = loop_counter = 0;
        SDL_CreateThread( limit_fps_thread, "limit_fps_thread()", NULL );
    }

    // if we throttled too much, cpu idle wait
    while( limit_fps_running && (loop_counter > timer_counter) ) {
        //thread_yield();
        SDL_DelayPrecise(100);
    }

    // max auto frameskip is 10: ie, even if speed is low paint at least one frame every 10
    enum { maxframeskip = 10 };
    if( timer_counter > loop_counter + maxframeskip ) {
        loop_counter = timer_counter;
    }
    loop_counter++;

    // only draw if we are fast enough, otherwise skip the frame
    return loop_counter >= timer_counter;
}

#endif
