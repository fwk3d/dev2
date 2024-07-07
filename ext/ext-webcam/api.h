API texture_t* webcam(bool enabled); // returns NULL if camera not available or not ready

#if CODE

// @fixme: color on Y/UV planar modes

texture_t* webcam(bool enabled) {
    static texture_t t = {0};
    static SDL_Camera *camera = 0;

    if( enabled ) {
        if( !camera ) {
            int devcount = 0;
            SDL_CameraID *devices = SDL_GetCameras(&devcount);
            if( devices && devcount ) {
                int numspecs = 0;
                SDL_CameraSpec **specs = SDL_GetCameraSupportedFormats(devices[0], &numspecs), *chosen = 0;
                if( specs && numspecs ) { // iterate the camera modes
                    for( int i = 0; i < numspecs; ++i ) {
                        // @fixme: discard non-pow2 resolutions
                        // @fixme: pick up spec mode that matches best our app resolution (~similar densities)
                        if( specs[i]->format == SDL_PIXELFORMAT_YUY2 ) continue;                     // not ok, probably interleaved
                        if( specs[i]->format == SDL_PIXELFORMAT_NV12 ) { chosen = specs[i]; break; } // ok Y + U/V (2 planes)
                    }
                    if( chosen ) {
                        camera = SDL_OpenCamera(devices[0], chosen);
                        SDL_free(devices);
                    }
                }
            }
        }
        if( !t.id ) {
            t = texture_create(1, 1, 1, NULL, TEXTURE_R);
        }

        SDL_Surface *frame = 0;
        uint64_t timestamp_ns = 0;
        if (t.id) for( frame = SDL_AcquireCameraFrame(camera, &timestamp_ns); frame; SDL_ReleaseCameraFrame(camera, frame), frame = 0) {
            // @fixme: optimize: bypass texture_update() by extracting WxH+GL handle from SDL_Surface (or SDL_Texture)
            int bpp = frame->pitch / frame->w;
            texture_update(&t,frame->w,frame->h,bpp,frame->pixels,TEXTURE_R);
        }
    } else {
        if( camera ) {
            SDL_CloseCamera(camera);
            camera = 0;
        }
        if( t.id ) {
            texture_destroy(&t);
            t.id = 0;
        }
    }

    return enabled && camera && t.id ? &t : NULL;
}

#endif
