// -----------------------------------------------------------------------------
// screen-space model material lookup

#if !CODE

typedef struct drawmat_t  {
    handle fbo_id;
    texture_t matprops; ///< R: metallic, G: roughness, B: ao
    texture_t normals;
    texture_t albedo;
    texture_t depth;
} drawmat_t;

API drawmat_t drawmat();
API void      drawmat_destroy(drawmat_t *lookup);
API void      drawmat_clear(drawmat_t *lookup);

#else

// -----------------------------------------------------------------------------
// screen-space model material lookup

drawmat_t drawmat() {
    int saved_vp[4];
    glGetIntegerv(GL_VIEWPORT, saved_vp);

    drawmat_t lookup = {0};
    lookup.matprops = texture_create(saved_vp[2], saved_vp[3], 4, NULL, TEXTURE_RGBA|TEXTURE_FLOAT);
    lookup.normals = texture_create(saved_vp[2], saved_vp[3], 3, NULL, TEXTURE_RGB);
    lookup.albedo = texture_create(saved_vp[2], saved_vp[3], 4, NULL, TEXTURE_RGBA);
    lookup.depth = texture_create(saved_vp[2], saved_vp[3], 1, NULL, TEXTURE_DEPTH);

    glGenFramebuffers(1, &lookup.fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, lookup.fbo_id);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lookup.matprops.id, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lookup.normals.id, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, lookup.albedo.id, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, lookup.depth.id, 0);

    GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, draw_buffers);

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if( GL_FRAMEBUFFER_COMPLETE != result ) {
        die("ERROR: Framebuffer not complete.");
    }

    return lookup;
}

void drawmat_destroy(drawmat_t *lookup) {
    texture_destroy(&lookup->matprops);
    texture_destroy(&lookup->normals);
    texture_destroy(&lookup->albedo);
    texture_destroy(&lookup->depth);
    glDeleteFramebuffers(1, &lookup->fbo_id);
}

void drawmat_clear(drawmat_t *lookup) {
    fbo_bind(lookup->fbo_id);
    glClearColor(0,0,0,0);
    viewport_clear(true, true);
    fbo_unbind();
}

void drawmat_render(drawmat_t *lookup, model_t m, mat44 proj, mat44 view, mat44 *models, unsigned count) {
    int saved_vp[4];
    glGetIntegerv(GL_VIEWPORT, saved_vp);

    if (lookup->depth.w != saved_vp[2] || lookup->depth.h != saved_vp[3]) {
        drawmat_destroy(lookup);
        *lookup = drawmat();
    }

    fbo_bind(lookup->fbo_id);
    model_render(&m, proj, view, models, count, RENDER_PASS_MATERIAL);
    fbo_unbind();
}

#endif
