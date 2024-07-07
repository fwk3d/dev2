// @todo: doppler: alDopplerFactor, alSpeedOfSound
// @todo: audio layers, mixer
// @todo: extra sample formats
// @todo: sts_mixer library { pitch, tempo, bpm }

#if !CODE

typedef struct audio_t {
    const char *err, *type, *path, *ptr; int bytes;

    unsigned id, bits, rate, channels, format;

    float duration;
    short *samples;
} audio_t;

enum AUDIO_FLAGS {
    AUDIO_CLIP = 0,
    AUDIO_STREAM = 1, // @todo
};

API audio_t audio( const void *ptr, int len, unsigned flags );
API    void audio_destroy(audio_t *);

// ----------------------------------------------------------------------------

typedef struct speaker_t {
    unsigned source;
    unsigned buffer;
} speaker_t;

API speaker_t speaker();
API void       speaker_play(speaker_t*, audio_t idx, bool paused);
API void        speaker_pause(speaker_t*, bool);
API void        speaker_loop(speaker_t*, bool on );
API void        speaker_gain(speaker_t*, float gain );
API void        speaker_pitch(speaker_t*, float pitch );
API void        speaker_position(speaker_t*, const float position[3], bool relative );
API void        speaker_velocity(speaker_t*, const float velocity[3] );
API void        speaker_direction(speaker_t*, const float direction[3] );
API void        speaker_attenuation(speaker_t*, float rollOff, float refDistance );
//API void        speaker_distance(speaker_t*, float mind, float maxd );
API void       speaker_stop(speaker_t*);
API bool       speaker_finished(speaker_t*);
API void      speaker_destroy(speaker_t*);

// ----------------------------------------------------------------------------

API void listener_gain( float gain );
API void listener_position( const float position[3] );
API void listener_velocity( const float velocity[3] );
API void listener_direction( const float direction[3] );

#else
// --------------------------------------------------------------------------

static vec3 al_coord3(const float coord[3]) {
    float flip[3] = { -coord[0], coord[1], coord[2] };
    return ptr3(flip);
}

void listener_gain( float gain ) {
    alListenerf( AL_GAIN, gain );
}

void listener_direction( const float direction[3] ) {
    vec3 orientation[2] = { al_coord3(direction), vec3(0,-1,0) }; // orientation { norm(at), norm(up) };
    alListenerfv( AL_ORIENTATION, &orientation[0].x );
}

void listener_position( const float position[3] ) {
    vec3 v = al_coord3(position);
    alListenerfv( AL_POSITION, &v.x );
}

void listener_velocity( const float velocity[3] ) {
    vec3 v = al_coord3(velocity);
    alListenerfv( AL_VELOCITY, &v.x );
}

// --------------------------------------------------------------------------

audio_t audio( const void *ptr, int len, unsigned flags ) {
    audio_t z = {0};
    if( !len ) {
        z.path = strdup((const char *)ptr);
        z.ptr = file_read(z.path, &len);
    }
    z.bytes = len;
    if( !len ) {
        return z.err = "audio(): no data", z;
    }

    alGenBuffers( 1, &z.id );

    // .ogg
    if( !z.type )
    for( stb_vorbis *oss = stb_vorbis_open_memory( (unsigned char *)z.ptr, z.bytes, NULL, NULL); oss; oss = 0) {
        z.type = ".ogg";

        stb_vorbis_info info = stb_vorbis_get_info(oss);

        int samples = (stb_vorbis_stream_length_in_samples(oss) * info.channels);
        z.channels = info.channels;
        z.bits = 16;
        z.format = info.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
        z.bytes = samples * sizeof(short);
        z.rate = info.sample_rate;

        short *data = (short *)malloc( z.bytes );

        stb_vorbis_get_samples_short_interleaved(oss, info.channels, data, samples );
        stb_vorbis_close(oss);

        free((void*)z.ptr);

        alBufferData( z.id, z.format, data, z.bytes, z.rate );

        ALint error;
        if ((error = alGetError()) != AL_NO_ERROR) {
            die("alBufferData() failed");
        }

        stb_vorbis_free(data);
    }

    if( !z.type ) {
        audio_destroy(&z);
        return z.err = "unsupported file format", z;
    }

    z.bits = 16;
    z.duration = (z.bytes/z.channels/(z.bits/8.)) / z.rate;

    return z;
}

void audio_destroy(audio_t *z) {
    if( z->id ) {
        alDeleteBuffers( 1, &z->id );
        z->id = 0;
    }
    if( z->path ) {
        free((void*)z->path);
        z->path = 0;
    }
}

// ----------------------------------------------------------------------------

speaker_t speaker() {
    speaker_t z = {0}, s = z;
    alGenSources( 1, &s.source );
    return s.source > 0 ? s : z;
}
static
void speaker_unbind(speaker_t *s) {
    if( !s->buffer )
        return;
    alSourcei( s->source, AL_BUFFER, 0 );
    s->buffer = 0;
}
static
bool speaker_bind(speaker_t *s, int buffer) {
    if( s->buffer )
        speaker_unbind(s);

    s->buffer = buffer;

    speaker_loop( s, false );
    alSourcei( s->source, AL_SOURCE_RELATIVE, AL_FALSE );

    alSourcei( s->source, AL_BUFFER, buffer );
    //alSourceQueue

    alSourcef( s->source, AL_MIN_GAIN, 0.0 );
    alSourcef( s->source, AL_MAX_GAIN, 1.0 );

    speaker_gain( s, 1 );
    speaker_pitch( s, 1 );

    // seeking : AL_SEC_OFFSET (secs), AL_SAMPLE_OFFSET (samps), AL_BYTE_OFFSET(bytes)
    // must be reset back on every loop
    // alSourcef( soundSource.source, AL_SEC_OFFSET, 5.0f );

    return true;
}
void speaker_pause( speaker_t *s, bool paused ) {
    (paused ? alSourcePause : alSourcePlay)( s->source );
}
bool speaker_finished( speaker_t *s ) {
    ALint state;
    alGetSourcei( s->source, AL_SOURCE_STATE, &state );
    return state != AL_PLAYING;
}
void speaker_play( speaker_t *s, audio_t au, bool paused ) {
    alSourceStop( s->source );
    speaker_unbind( s );
    speaker_bind( s, au.id );
    speaker_pause(s, paused);
}
void speaker_destroy(speaker_t *s) {
    speaker_unbind( s );
    if( !s->source )
        return;
    alDeleteSources( 1, &s->source );
    s->source = 0;
}

void speaker_loop( speaker_t *s, bool on ) {
    alSourcei( s->source, AL_LOOPING, on ? AL_TRUE : AL_FALSE );
}
void speaker_gain( speaker_t *s, float gain ) {
    alSourcef( s->source, AL_GAIN, gain );
}
void speaker_pitch( speaker_t *s, float pitch ) {  // [0.0001f .. N]
    pitch += !pitch * 0.0001f;
    alSourcef( s->source, AL_PITCH, pitch );
}
void speaker_position( speaker_t *s, const float position[3], bool relative ) {
    vec3 v = al_coord3(position);
    alSourcefv( s->source, AL_POSITION, &v.x );
    alSourcei( s->source, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE );
}
void speaker_velocity( speaker_t *s, const float velocity[3] ) {
    vec3 v = al_coord3(velocity);
    alSourcefv( s->source, AL_VELOCITY, &v.x );
}
void speaker_direction( speaker_t *s, const float direction[3] ) {
    vec3 v = al_coord3(direction);
    alSourcefv( s->source, AL_DIRECTION, &v.x );
}
void speaker_attenuation( speaker_t *s, float rollOff, float refDistance ) {
    alSourcef( s->source, AL_REFERENCE_DISTANCE, refDistance );
    alSourcef( s->source, AL_ROLLOFF_FACTOR, rollOff );
}
/*
void speaker_distance( speaker_t s, float mind, float maxd ) {
    alSourcef( s.source, AL_REFERENCE_DISTANCE, mind );
    alSourcef( s.source, AL_MAX_DISTANCE, maxd );
}
*/

// ----------------------------------------------------------------------------

ALCdevice *aldev = 0;
ALCcontext *alctx = 0;

static
const char* aldevice_name( int devnum ) {
    if( !!alcIsExtensionPresent( NULL, (const ALCchar *)"ALC_ENUMERATION_EXT" ) ) {
        const char *devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER); // ALC_ALL_DEVICES_SPECIFIER);
        for( ; *devices ; devices += strlen(devices) + 1 ) {
            if( devnum-- == 0 ) return devices;
        }
    }
    return NULL;
}

static
int aldevice_init( int devnum ) {
    // find device
    const char *usedev = aldevice_name(devnum);

    // select device
    aldev = alcOpenDevice( usedev );
    if( !aldev ) die("cannot open AL device `%s`", usedev);

    // efx device capabilities
    bool has_efx = !!alcIsExtensionPresent(aldev, "ALC_EXT_EFX");
    if( has_efx ) {
        load_efx();
    }

    ALint attribs[4] = { 0 };
    attribs[0] = 0x20003; // ALC_MAX_AUXILIARY_SENDS;
    attribs[1] = 4;

    // create context
    alctx = alcCreateContext(aldev, has_efx ? attribs : NULL );
    if( !alctx ) die("cannot create AL context (efx:%d)", has_efx);

    alcMakeContextCurrent(alctx);

    do_once {
        // debug
        // if (alGetString(AL_VERSION)) printf("OpenAL version: %s\n", alGetString(AL_VERSION));
        // if (alGetString(AL_RENDERER)) printf("OpenAL renderer: %s\n", alGetString(AL_RENDERER));
        // if (alGetString(AL_VENDOR)) printf("OpenAL vendor: %s\n", alGetString(AL_VENDOR));
        // if (alGetString(AL_EXTENSIONS)) printf("OpenAL extensions: %s\n", alGetString(AL_EXTENSIONS));

        //alDistanceModel( AL_INVERSE_DISTANCE );
        alDistanceModel( AL_INVERSE_DISTANCE_CLAMPED );
        listener_gain( 1.0 );
    }

    return (has_efx << 1) | 1;
}

#if 0
const char *al_error( ALenum error ) {
    /**/ if( error == AL_NO_ERROR ) return 0;
    else if( error == AL_INVALID_NAME) return "AL_INVALID_NAME";
    else if( error == AL_INVALID_ENUM) return "AL_INVALID_ENUM";
    else if( error == AL_INVALID_VALUE) return "AL_INVALID_VALUE";
    else if( error == AL_INVALID_OPERATION) return "AL_INVALID_OPERATION";
    else if( error == AL_OUT_OF_MEMORY) return "AL_OUT_OF_MEMORY";
    return "AL UNKNOWN ERROR";
}
#endif

void al_quit(void) {
	// @todo: stop all speakers. needed?
    ALCcontext *ctx = alcGetCurrentContext();
    if( ctx ) {
        ALCdevice *device = alcGetContextsDevice(ctx);
        if( device ) {
            alcCloseDevice(device);
        }
        alcMakeContextCurrent(NULL);
        alcDestroyContext(ctx);
    }
    if( aldev ) {
//        alcCloseDevice(aldev);
        aldev = 0;
    }
}

int al_init() {
    do_once aldevice_init(0), atexit(al_quit); //, printf("Using audio device: %s\n", aldevice_name(0));
    return 1;
}

AUTORUN { al_init(); }

#endif
