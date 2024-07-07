// -----------------------------------------------------------------------------
// fullscreen quads

#if !CODE

typedef enum BLIT_FLAGS {
    BLIT_RGB = 1,
    BLIT_YCBCR = 2,
    BLIT_Y = 4,
    BLIT_FLIP = 8,
} BLIT_FLAGS;

API void blit( texture_t *textures, float gamma, unsigned flags );

API void blit_rgb_gamma( texture_t texture_rgb, float gamma );
API void blit_rgb_flipped_gamma( texture_t texture_rgb, float gamma );
API void blit_rgb( texture_t texture_rgb );
API void blit_rgb_flipped( texture_t texture );
API void blit_y( texture_t texture_Y );
API void blit_ycbcr( texture_t texture_YCbCr[3] );
API void blit_ycbcr_flipped( texture_t texture_YCbCr[3] );

#else

void blit( texture_t *textures, float gamma, unsigned flags ) {
    if( flags & BLIT_YCBCR ) {
        ( flags & BLIT_FLIP ? blit_ycbcr_flipped : blit_ycbcr)( textures );
    }
    else if( flags & BLIT_Y ) {
        blit_y( textures[0] );
    }
    else {
        if( gamma )
        ( flags & BLIT_FLIP ? blit_rgb_flipped_gamma : blit_rgb_gamma)( textures[0], gamma );
        else
        ( flags & BLIT_FLIP ? blit_rgb_flipped : blit_rgb)( textures[0] );
    }
}

// usage: bind empty vao & commit call for 6 (quad) or 3 vertices (tri).
// ie, glBindVertexArray(empty_vao); glDrawArrays(GL_TRIANGLES, 0, 3);

static renderstate_t blit_rs;

static inline
void blit_rs_init() {
    do_once {
        blit_rs = renderstate();
        blit_rs.depth_test_enabled = false;
        blit_rs.blend_enabled = true;
        blit_rs.blend_src = GL_SRC_ALPHA;
        blit_rs.blend_dst = GL_ONE_MINUS_SRC_ALPHA;
        blit_rs.front_face = GL_CW;
    }
}

static
void blit_rgb_gamma_rs( texture_t texture, float gamma, renderstate_t rs ) {
    static int program = -1, vao = -1, u_inv_gamma = -1;
    if( program < 0 ) {
        const char* vs = file_read("shaders/fullscreen_quad_B_flipped_vs.gl", 0);
        const char* fs = file_read("shaders/texel_inv_gamma_fs.gl", 0);

        program = shader(vs, fs, "", "fragcolor" , NULL);
        u_inv_gamma = glGetUniformLocation(program, "u_inv_gamma");
        glGenVertexArrays( 1, (GLuint*)&vao );
    }

    GLenum texture_type = texture.flags & TEXTURE_ARRAY ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
    renderstate_apply(&rs);
    glUseProgram( program );
    float inv_gamma = 1.0f / gamma;
    glUniform1f( u_inv_gamma, inv_gamma );

    glBindVertexArray( vao );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( texture_type, texture.id );

    glDrawArrays( GL_TRIANGLES, 0, 6 );
    profile_incstat("Render.num_drawcalls", +1);
    profile_incstat("Render.num_triangles", +2);

    glBindTexture( texture_type, 0 );
    glBindVertexArray( 0 );
    glUseProgram( 0 );
}
void blit_rgb_gamma( texture_t texture, float gamma ) {
    blit_rs_init();
    blit_rgb_gamma_rs(texture, gamma, blit_rs);
}
static
void blit_rgb_flipped_gamma_rs( texture_t texture, float gamma, renderstate_t rs ) {
    static int program = -1, vao = -1, u_inv_gamma = -1;
    if( program < 0 ) {
        const char* vs = file_read("shaders/fullscreen_quad_B_vs.gl", 0);
        const char* fs = file_read("shaders/texel_inv_gamma_fs.gl", 0);

        program = shader(vs, fs, "", "fragcolor" , NULL);
        u_inv_gamma = glGetUniformLocation(program, "u_inv_gamma");
        glGenVertexArrays( 1, (GLuint*)&vao );
    }

    GLenum texture_type = texture.flags & TEXTURE_ARRAY ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
    renderstate_apply(&rs);
    glUseProgram( program );
    float inv_gamma = 1.0f / gamma;
    glUniform1f( u_inv_gamma, inv_gamma );

    glBindVertexArray( vao );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( texture_type, texture.id );

    glDrawArrays( GL_TRIANGLES, 0, 6 );
    profile_incstat("Render.num_drawcalls", +1);
    profile_incstat("Render.num_triangles", +2);

    glBindTexture( texture_type, 0 );
    glBindVertexArray( 0 );
    glUseProgram( 0 );
}
void blit_rgb_flipped_gamma( texture_t texture, float gamma ) {
    blit_rs_init();
    blit_rgb_flipped_gamma_rs(texture, gamma, blit_rs);
}

void blit_rgb( texture_t texture_rgb ) {
    blit_rgb_gamma(texture_rgb, 1.0f);
}

void blit_rgb_flipped( texture_t texture ) {
    blit_rgb_flipped_gamma(texture, 1.0f);
}

void blit_ycbcr( texture_t textureYCbCr[3] ) {
    blit_rs_init();
    static int program = -1, vao = -1, u_gamma = -1, uy = -1, ucb = -1, ucr = -1;
    if( program < 0 ) {
        const char* vs = file_read("shaders/fullscreen_quad_B_flipped_vs.gl", 0);
        const char* fs = file_read("shaders/texel_ycbr_gamma_saturation_fs.gl", 0);

        program = shader(vs, fs, "", "fragcolor" , NULL);
        u_gamma = glGetUniformLocation(program, "u_gamma");

        uy = glGetUniformLocation(program, "u_texture_y");
        ucb = glGetUniformLocation(program, "u_texture_cb");
        ucr = glGetUniformLocation(program, "u_texture_cr");

        glGenVertexArrays( 1, (GLuint*)&vao );
    }

    renderstate_apply(&blit_rs);
    glUseProgram( program );
    // glUniform1f( u_gamma, gamma );

    glBindVertexArray( vao );

    glUniform1i(uy, 0);
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, textureYCbCr[0].id );

    glUniform1i(ucb, 1);
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, textureYCbCr[1].id );

    glUniform1i(ucr, 2);
    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, textureYCbCr[2].id );

    glDrawArrays( GL_TRIANGLES, 0, 6 );
    profile_incstat("Render.num_drawcalls", +1);
    profile_incstat("Render.num_triangles", +2);

    glBindTexture( GL_TEXTURE_2D, 0 );
    glBindVertexArray( 0 );
    glUseProgram( 0 );
//    glDisable( GL_BLEND );
}

void blit_ycbcr_flipped( texture_t textureYCbCr[3] ) {
    blit_rs_init();
    static int program = -1, vao = -1, u_gamma = -1, uy = -1, ucb = -1, ucr = -1;
    if( program < 0 ) {
        const char* vs = file_read("shaders/fullscreen_quad_B_vs.gl", 0);
        const char* fs = file_read("shaders/texel_ycbr_gamma_saturation_fs.gl", 0);

        program = shader(vs, fs, "", "fragcolor" , NULL);
        u_gamma = glGetUniformLocation(program, "u_gamma");

        uy = glGetUniformLocation(program, "u_texture_y");
        ucb = glGetUniformLocation(program, "u_texture_cb");
        ucr = glGetUniformLocation(program, "u_texture_cr");

        glGenVertexArrays( 1, (GLuint*)&vao );
    }

    renderstate_apply(&blit_rs);
    glUseProgram( program );
    // glUniform1f( u_gamma, gamma );

    glBindVertexArray( vao );

    glUniform1i(uy, 0);
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, textureYCbCr[0].id );

    glUniform1i(ucb, 1);
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, textureYCbCr[1].id );

    glUniform1i(ucr, 2);
    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, textureYCbCr[2].id );

    glDrawArrays( GL_TRIANGLES, 0, 6 );
    profile_incstat("Render.num_drawcalls", +1);
    profile_incstat("Render.num_triangles", +2);

    glBindTexture( GL_TEXTURE_2D, 0 );
    glBindVertexArray( 0 );
    glUseProgram( 0 );
//    glDisable( GL_BLEND );
}

void blit_y( texture_t textureY ) {
    blit_rs_init();
    static int program = -1, vao = -1, u_gamma = -1, uy = -1;
    if( program < 0 ) {
        const char* vs = file_read("shaders/fullscreen_quad_B_flipped_vs.gl", 0);
        const char* fs = file_read("shaders/texel_y_gamma_saturation_fs.gl", 0);

        program = shader(vs, fs, "", "fragcolor" , NULL);
        u_gamma = glGetUniformLocation(program, "u_gamma");

        uy = glGetUniformLocation(program, "u_texture_y");

        glGenVertexArrays( 1, (GLuint*)&vao );
    }

    renderstate_apply(&blit_rs);
    glUseProgram( program );
    // glUniform1f( u_gamma, gamma );

    glBindVertexArray( vao );

    glUniform1i(uy, 0);
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, textureY.id );

    glDrawArrays( GL_TRIANGLES, 0, 6 );
    profile_incstat("Render.num_drawcalls", +1);
    profile_incstat("Render.num_triangles", +2);

    glBindTexture( GL_TEXTURE_2D, 0 );
    glBindVertexArray( 0 );
    glUseProgram( 0 );
//    glDisable( GL_BLEND );
}

#endif
