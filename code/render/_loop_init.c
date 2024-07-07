    // init subsystems that DO depend on cooked assets now

    #pragma omp parallel for
    for( i = 0; i <= 3; ++i ) {
        if(i == 0) scene_init(); // init these on thread #0, since both will be compiling shaders, and shaders need to be compiled from the very same thread than glfwMakeContextCurrent() was set up
        if(i == 1) audio_init(0);
        if(i == 2) kit_init(), midi_init();
        if(i == 3) network_init();
    }
