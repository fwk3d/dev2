// -----------------------------------------------------------------------------
// brdf

API texture_t brdf_lut();

#if CODE

// -----------------------------------------------------------------------------
// brdf

static texture_t brdf = {0};

static void brdf_load() {
    // generate texture
    unsigned tex;
    glGenTextures(1, &tex);

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

    brdf.id = tex;
    brdf.w = 512;
    brdf.h = 512;

    // create program and generate BRDF LUT
    unsigned lut_fbo = fbo_id(tex, 0, 0), rbo=0;
    fbo_bind(lut_fbo);

    static int program = -1, vao = -1;
    if( program < 0 ) {
        const char* vs = file_read("shaders/fullscreen_quad_B_flipped_vs.gl", 0);
        const char* fs = file_read("shaders/brdf_lut.gl", 0);

        program = shader(vs, fs, "", "fragcolor", NULL);
        glGenVertexArrays( 1, (GLuint*)&vao );
    }

    glDisable(GL_BLEND);

#if 1 //< @r-lyeh. use this?
    GLint old_shader = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &old_shader);
#else
    int old_shader = last_shader;
#endif

    glUseProgram( program );

    glViewport(0, 0, 512, 512);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glBindVertexArray( vao );

    glDrawArrays( GL_TRIANGLES, 0, 6 );
    profile_incstat("Render.num_drawcalls", +1);
    profile_incstat("Render.num_triangles", +2);

    glBindVertexArray( 0 );

    glUseProgram( old_shader );

    fbo_unbind();
    fbo_destroy_id(lut_fbo);
}

texture_t brdf_lut() {
    do_once brdf_load();
    return brdf;
}

#endif
