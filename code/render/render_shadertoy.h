// -----------------------------------------------------------------------------
// shadertoys

#if !CODE

enum {
    SHADERTOY_FLIP_Y = 2,
    SHADERTOY_IGNORE_FBO = 4,
    SHADERTOY_IGNORE_MOUSE = 8,
};

typedef struct shadertoy_t {
    handle vao, program;
    int uniforms[32];
    int texture_channels[4];
    int frame;
    uint64_t t;
    texture_t tx;
    vec2i dims;
    int flags;
    vec4 mouse;
} shadertoy_t;

API shadertoy_t  shadertoy( const char *shaderfile, unsigned flags );
API shadertoy_t* shadertoy_render( shadertoy_t *s, float delta );

#else

// ----------------------------------------------------------------------------
// shadertoys
//
// @todo: multipass
// - https://www.shadertoy.com/view/Mst3Wr - la calanque
// - https://www.shadertoy.com/view/XsyGWV - sirenian dawn
// - https://www.shadertoy.com/view/Xst3zX - wordtoy
// - https://www.shadertoy.com/view/MddGzf - bricks game
// - https://www.shadertoy.com/view/Ms33WB - post process - ssao
// - https://www.shadertoy.com/view/Xds3zN

enum shadertoy_uniforms {
    iFrame,
    iTime,
    iDate,
    iGlobalTime,
    iGlobalFrame,
    iGlobalDelta,
    iChannel0,
    iChannel1,
    iChannel2,
    iChannel3,
    iResolution,
    iMouse,
    iOffset,
    iSampleRate,
    iChannelResolution,
    iChannelTime,
    // iCameraScreen
    // iCameraPosition
    // iCameraActive
};

shadertoy_t shadertoy( const char *shaderfile, unsigned flags ) {
    shadertoy_t s = {0};
    s.flags = flags;

    char *file = file_read(shaderfile,0);
    if( !file ) return s;

    glGenVertexArrays(1, &s.vao);

    char *fs = stringf("%s%s", file_read("shaders/header_shadertoy.gl",0), file);
    s.program = shader((flags&SHADERTOY_FLIP_Y) ? file_read("shaders/shadertoy_flip_vs.gl",0) : file_read("shaders/shadertoy_vs.gl",0), fs, "", "fragColor", NULL);
    FREE(fs);

    if( strstr(file, "noise3.jpg"))
    s.texture_channels[0] = texture("shadertoys/tex12.png", 0).id;
    else
    s.texture_channels[0] = texture("shadertoys/tex04.jpg", 0).id;

    s.uniforms[iFrame] = glGetUniformLocation(s.program, "iFrame");
    s.uniforms[iTime] = glGetUniformLocation(s.program, "iTime");
    s.uniforms[iDate] = glGetUniformLocation(s.program, "iDate");
    s.uniforms[iGlobalTime] = glGetUniformLocation(s.program, "iGlobalTime");
    s.uniforms[iGlobalDelta] = glGetUniformLocation(s.program, "iGlobalDelta");
    s.uniforms[iGlobalFrame] = glGetUniformLocation(s.program, "iGlobalFrame");
    s.uniforms[iResolution] = glGetUniformLocation(s.program, "iResolution");

    s.uniforms[iChannel0] = glGetUniformLocation(s.program, "iChannel0");
    s.uniforms[iChannel1] = glGetUniformLocation(s.program, "iChannel1");
    s.uniforms[iChannel2] = glGetUniformLocation(s.program, "iChannel2");
    s.uniforms[iChannel3] = glGetUniformLocation(s.program, "iChannel3");

    s.uniforms[iMouse] = glGetUniformLocation(s.program, "iMouse");
    s.uniforms[iOffset] = glGetUniformLocation(s.program, "iOffset");
    s.uniforms[iSampleRate] = glGetUniformLocation(s.program, "iSampleRate");
    s.uniforms[iChannelResolution] = glGetUniformLocation(s.program, "iChannelResolution");
    s.uniforms[iChannelTime] = glGetUniformLocation(s.program, "iChannelTime");

    return s;
}

shadertoy_t* shadertoy_render(shadertoy_t *s, float delta) {
    int saved_vp[4];
    glGetIntegerv(GL_VIEWPORT, saved_vp);
    if( s->program && s->vao ) {
        if( s->dims.x && !(s->flags&SHADERTOY_IGNORE_FBO) && !texture_rec_begin(&s->tx, s->dims.x, s->dims.y) ) {
            return s;
        }

        if(input_down(MOUSE_L) || input_down(MOUSE_R) ) s->mouse.z = input(MOUSE_X), s->mouse.w = -(window_height() - input(MOUSE_Y));
        if(input(MOUSE_L) || input(MOUSE_R)) s->mouse.x = input(MOUSE_X), s->mouse.y = (window_height() - input(MOUSE_Y));
        vec4 m = mul4(s->mouse, vec4(1,1,1-2*(!input(MOUSE_L) && !input(MOUSE_R)),1-2*(input_down(MOUSE_L) || input_down(MOUSE_R))));

        time_t tmsec = time(0);
        struct tm *tm = localtime(&tmsec);
        s->t += delta * 1000;

        glUseProgram(s->program);
        glUniform1f(s->uniforms[iGlobalTime], s->t / 1000.f );
        glUniform1f(s->uniforms[iGlobalFrame], s->frame++);
        glUniform1f(s->uniforms[iGlobalDelta], delta / 1000.f );
        glUniform2f(s->uniforms[iResolution], s->dims.x ? s->dims.x : window_width(), s->dims.y ? s->dims.y : window_height());
        if (!(s->flags&SHADERTOY_IGNORE_MOUSE)) glUniform4f(s->uniforms[iMouse], m.x,m.y,m.z,m.w );

        glUniform1i(s->uniforms[iFrame], (int)window_frame());
        glUniform1f(s->uniforms[iTime], time_ss());
        glUniform4f(s->uniforms[iDate], tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_sec + tm->tm_min * 60 + tm->tm_hour * 3600);

        int unit = 0;
        for( int i = 0; i < 4; i++ ) {
            if( s->texture_channels[i] ) {
                glActiveTexture(GL_TEXTURE0 + unit);
                glBindTexture(GL_TEXTURE_2D, s->texture_channels[i]);
                glUniform1i(s->uniforms[iChannel0+i], unit);
                unit++;
            }
        }

        glViewport(0, 0, s->dims.x ? s->dims.x : window_width(), s->dims.y ? s->dims.y : window_height());
        glBindVertexArray(s->vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        if(s->dims.x && !(s->flags&SHADERTOY_IGNORE_FBO)) texture_rec_end(&s->tx); // texture_rec
    }
    glViewport(saved_vp[0], saved_vp[1], saved_vp[2], saved_vp[3]);
    return s;
}

#endif
