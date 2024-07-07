// -----------------------------------------------------------------------------
// compute shaders

#if !CODE

enum BUFFER_USAGE {
    STATIC_DRAW,
    STATIC_READ,
    STATIC_COPY,

    DYNAMIC_DRAW,
    DYNAMIC_READ,
    DYNAMIC_COPY,

    STREAM_DRAW,
    STREAM_READ,
    STREAM_COPY
};

enum BUFFER_MODE {
    BUFFER_READ,
    BUFFER_WRITE,
    BUFFER_READ_WRITE
};

/// Loads the compute shader and compiles a GL program.
/// return: GL program, 0 if failed.
/// cs: shader source code
API unsigned compute(const char *cs);

/// Runs the compute program with provided global workgroup size on x y z grid.
/// wx: global workgroup size x
/// wy: global workgroup size y
/// wz: global workgroup size z
API void compute_dispatch(unsigned wx, unsigned wy, unsigned wz);

/// Binds a texture to the program
/// !!! Set `layer` to -1 to disable layered access.
/// t: texture to bind
/// unit: texture unit bind index
/// level: texture level access (MIP0, MIP1, ...)
/// layer: bind layer
/// access: texture access policy
/// see: BUFFER_MODE
API void shader_image(texture_t t, unsigned unit, unsigned level, int layer, unsigned access);

/// Binds a texture to the program
/// !!! Set `layer` to -1 to disable layered access.
/// texture: GL texture handle
/// unit: texture unit bind index
/// level: texture level access (MIP0, MIP1, ...)
/// layer: bind layer
/// texel_type: image texel format (RGBA8, RGBA32F, ...)
/// access: texture access policy
/// see: BUFFER_MODE
API void shader_image_unit(unsigned texture, unsigned unit, unsigned level, int layer, unsigned texel_type, unsigned access);

// gpu memory barriers

/// Blocks main thread until all memory operations are done by the GPU.
API void write_barrier();

/// Blocks main thread until all image operations are done by the GPU.
API void write_barrier_image();

#else

// ----------------------------------------------------------------------------
// compute shaders

unsigned compute(const char *cs){
    #if is(ems)
    return 0;
    #else
    PRINTF(/*"!"*/"Compiling compute shader\n");

    cs = cs[0] == '#' && cs[1] == 'c' ? cs : va("#version 450 core\n%s", cs ? cs : "");

    GLuint comp = shader_compile(GL_COMPUTE_SHADER, cs);
    GLuint program = 0;

    if( comp ) {
        program = glCreateProgram();

        glAttachShader(program, comp);

        glLinkProgram(program);

        GLint status = GL_FALSE, length;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        char *buf = 0;
#ifdef DEBUG_SHADER
        if (status != GL_FALSE && program == DEBUG_SHADER) {
#else
        if (status == GL_FALSE) {
#endif
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
            buf = stack(length+1);
            glGetProgramInfoLog(program, length, NULL, buf);
            puts("--- cs:");
            shader_print(cs);
        }
        if (status == GL_FALSE) {
            die("ERROR: shader(): Shader/program link: %s\n", buf ? buf : "(null)");
            return 0;
        }

        glDeleteShader(comp);
    }
    return program;
    #endif
}

void compute_dispatch(unsigned wx, unsigned wy, unsigned wz){
    glDispatchCompute(wx, wy, wz);
}

void write_barrier(){
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void write_barrier_image(){
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void shader_destroy(unsigned program){
    if( program == ~0u ) return;
    glDeleteProgram(program);
// if(s->name) FREE(s->name), s->name = NULL;
}

#endif
