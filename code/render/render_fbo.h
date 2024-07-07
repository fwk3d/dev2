// -----------------------------------------------------------------------------
// fbos

#if !CODE

enum FBO_FLAGS {
    FBO_NO_DEPTH = 1,
    FBO_NO_COLOR = 2,
};

enum FBO_BLIT_MODE {
    FBO_BLIT_COPY,
    FBO_BLIT_ADDITIVE,
    FBO_BLIT_COPY_ALPHA,
};

typedef struct fbo_t {
    unsigned id;
    texture_t texture_color;
    texture_t texture_depth;

    // internal
    unsigned width, height;
    int flags;
    int texture_flags;
} fbo_t;

API fbo_t fbo(unsigned width, unsigned height, int flags, int texture_flags);
API unsigned fbo_id(unsigned texture_color, unsigned texture_depth, int flags);
API void     fbo_resize(fbo_t *f, unsigned width, unsigned height);
API void     fbo_attach(unsigned id, int slot, texture_t texture);
API void     fbo_attach_depth(unsigned id, texture_t texture);
API void     fbo_bind(unsigned id);
API void     fbo_unbind();
API void     fbo_destroy(fbo_t f);
API void     fbo_destroy_id(unsigned id);
API void     fbo_blit(unsigned id, texture_t texture, int mode);

#else

// -----------------------------------------------------------------------------
// fbos

fbo_t fbo( unsigned width, unsigned height, int flags, int texture_flags ) {
    texture_t color_tex = {0}; 
    if (!(flags&FBO_NO_COLOR)) 
        color_tex = texture_create(width, height, 4, NULL, texture_flags);
    
    texture_t depth_tex = {0}; 
    if (!(flags&FBO_NO_DEPTH)) 
        depth_tex = texture_create(width, height, 4, NULL, TEXTURE_FLOAT|TEXTURE_DEPTH);

    fbo_t f = {0};
    f.id = fbo_id(color_tex.id, depth_tex.id, flags);
    f.texture_color = color_tex;
    f.texture_depth = depth_tex;
    f.width = width;
    f.height = height;
    f.flags = flags;
    f.texture_flags = texture_flags;
    return f;
}

static inline
void fbo_check_attachments(unsigned id) {
    int last_fb;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &last_fb);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    switch (glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
        case GL_FRAMEBUFFER_COMPLETE: break;
        case GL_FRAMEBUFFER_UNDEFINED: die("GL_FRAMEBUFFER_UNDEFINED");
        case GL_FRAMEBUFFER_UNSUPPORTED: die("GL_FRAMEBUFFER_UNSUPPORTED");
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: die("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: die("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: die("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: die("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
//      case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: die("GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT");
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: die("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
//      case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: die("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT");
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: die("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
        default: die("ERROR: Framebuffer not complete. glCheckFramebufferStatus returned %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
    glBindFramebuffer(GL_FRAMEBUFFER, last_fb);
}

unsigned fbo_id(unsigned color_texture_id, unsigned depth_texture_id, int flags) {
    int last_fb;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &last_fb);

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    if( color_texture_id ) glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture_id, 0);
    else glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
    if( depth_texture_id ) glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture_id, 0);
#if 0 // this is working; it's just not enabled for now
    else {
        // Extract texture dimensions from color_texture_id
        GLint color_width = 0, color_height = 0;
        glBindTexture(GL_TEXTURE_2D, color_texture_id);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &color_width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &color_height);
        glBindTexture(GL_TEXTURE_2D, 0);
        // create a non-sampleable renderbuffer object for depth and stencil attachments
        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, color_width, color_height); // use a single renderbuffer object for both a depth AND stencil buffer.
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    }
#endif

#if is(ems)
    GLenum nones[] = { GL_NONE };
    if(flags&FBO_NO_COLOR) glDrawBuffers(1, nones);
    if(flags&FBO_NO_COLOR) glReadBuffer(GL_NONE);
#else
    if(flags&FBO_NO_COLOR) glDrawBuffer(GL_NONE);
    if(flags&FBO_NO_COLOR) glReadBuffer(GL_NONE);
#endif

    if (color_texture_id) {
        fbo_check_attachments(fbo);
    }

    glBindFramebuffer (GL_FRAMEBUFFER, last_fb);
    return fbo;
}

void fbo_resize(fbo_t *f, unsigned width, unsigned height) {
    ASSERT(f);
    if (f->width == width && f->height == height) return;
    f->width = width;
    f->height = height;

    fbo_destroy(*f);
    *f = fbo(width, height, f->flags, f->texture_flags);
}

API void fbo_attach(unsigned id, int slot, texture_t texture) {
    int last_fb;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &last_fb);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+slot, GL_TEXTURE_2D, texture.id, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, last_fb);
    if (texture.id) fbo_check_attachments(id);
}
void fbo_attach_depth(unsigned id, texture_t texture) {
    int last_fb;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &last_fb);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture.id, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, last_fb);
    if (texture.id) fbo_check_attachments(id);
}

static __thread array(handle) fbos;
void fbo_bind(unsigned id) {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    array_push(fbos, id);
}
void fbo_unbind() {
    handle id = 0;
    if (array_count(fbos)) {
        array_pop(fbos);
        id = *array_back(fbos);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}
void fbo_destroy(fbo_t f) {
    fbo_destroy_id(f.id);
    if (f.texture_color.id) texture_destroy(&f.texture_color);
    if (f.texture_depth.id) texture_destroy(&f.texture_depth);
}
void fbo_destroy_id(unsigned id) {
    // glDeleteRenderbuffers(1, &renderbuffer);
    glDeleteFramebuffers(1, &id);
}

static renderstate_t fbo_blit_state;

void fbo_blit(unsigned id, texture_t texture, int mode) {
    do_once {
        fbo_blit_state = renderstate();
    }
    fbo_blit_state.depth_test_enabled = false;
    fbo_blit_state.front_face = GL_CW;
    fbo_blit_state.blend_enabled = true;
    fbo_blit_state.blend_src = GL_SRC_ALPHA;
    fbo_blit_state.blend_dst = GL_ONE_MINUS_SRC_ALPHA;
    fbo_blit_state.blend_src_alpha = GL_ONE;
    fbo_blit_state.blend_dst_alpha = GL_ZERO;

    if (mode == FBO_BLIT_ADDITIVE) {
        fbo_blit_state.blend_src = GL_ONE;
        fbo_blit_state.blend_dst = GL_ONE;
    }
    fbo_bind(id);
        blit_rgb_flipped_gamma_rs(texture, 1.0, fbo_blit_state);
    fbo_unbind();
}

#endif
