// -----------------------------------------------------------------------------
// cubemaps

#if !CODE

typedef struct cubemap_t {
    unsigned id;    // texture id
    vec3 sh[9];     // precomputed spherical harmonics coefficients

    // bake data
    int framebuffers[6];
    int textures[6];
    int depth_buffers[6];
    unsigned width, height;
    float *pixels;
    int step;
    vec3 pos;
} cubemap_t;

API cubemap_t  cubemap( texture_t texture, int flags ); // 1 equirectangular panorama
API void       cubemap_destroy(cubemap_t *c);
API cubemap_t* cubemap_get_active();
API void       cubemap_beginbake(cubemap_t *c, vec3 pos, unsigned width, unsigned height);
API bool       cubemap_stepbake(cubemap_t *c, mat44 proj /* out */, mat44 view /* out */);
API void       cubemap_endbake(cubemap_t *c, int step /* = 16 */, float sky_intensity /* = 1.0f */);
API void       cubemap_sh_reset(cubemap_t *c);
API void       cubemap_sh_addlight(cubemap_t *c, vec3 light, vec3 dir, float strength);

// lighting probe blending
API void       cubemap_sh_blend(vec3 pos, float max_dist, unsigned count, cubemap_t *probes, vec3 out_sh[9]);

#else

cubemap_t cubemap( texture_t texture, int flags ) {
    cubemap_t c = {0};
    c.id = texture.id;
    return c;
}

void cubemap_destroy(cubemap_t *c) {
    glDeleteTextures(1, &c->id);
    c->id = 0; // do not destroy SH coefficients still. they might be useful in the future.

    if (c->pixels) {
        FREE(c->pixels);
        glDeleteFramebuffers(6, c->framebuffers);
        glDeleteTextures(6, c->textures);
        glDeleteRenderbuffers(6, c->depth_buffers);
    }
}

static cubemap_t *last_cubemap;

cubemap_t* cubemap_get_active() {
    return last_cubemap;
}

// cubemap baker

static int sky_last_fb;
static int sky_last_vp[4];
void cubemap_beginbake(cubemap_t *c, vec3 pos, unsigned width, unsigned height) {
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &sky_last_fb);
    glGetIntegerv(GL_VIEWPORT, sky_last_vp);
    c->step = 0;
    c->pos = pos;

    if (!c->pixels || (c->width != width || c->height != height)) {
        c->pixels = REALLOC(c->pixels, width*height*12);
        c->width = width;
        c->height = height;

        if (c->framebuffers[0]) {
            glDeleteFramebuffers(6, c->framebuffers);
            glDeleteTextures(6, c->textures);
            glDeleteRenderbuffers(6, c->depth_buffers);
            for(int i = 0; i < 6; ++i) {
                c->framebuffers[i] = 0;
            }
        }
    }

    if (!c->framebuffers[0]) {
        for(int i = 0; i < 6; ++i) {
            glGenFramebuffers(1, &c->framebuffers[i]);
            glBindFramebuffer(GL_FRAMEBUFFER, c->framebuffers[i]);
            
            glGenTextures(1, &c->textures[i]);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, c->textures[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, c->textures[i], 0);

            // attach depth buffer
            glGenRenderbuffers(1, &c->depth_buffers[i]);
            glBindRenderbuffer(GL_RENDERBUFFER, c->depth_buffers[i]);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, c->depth_buffers[i]);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }
    }
}

bool cubemap_stepbake(cubemap_t *c, mat44 proj /* out */, mat44 view /* out */) {
    if (c->step >= 6) return false;

    static vec3 directions[6] = {{ 1, 0, 0},{-1, 0, 0},{ 0, 1, 0},{ 0,-1, 0},{ 0, 0, 1},{ 0, 0,-1}};
    static vec3 up_vectors[6] = {{ 0,-1, 0},{ 0,-1, 0},{ 0, 0, 1},{ 0, 0,-1},{ 0,-1, 0},{ 0,-1, 0}};

    glBindFramebuffer(GL_FRAMEBUFFER, c->framebuffers[c->step]);
    glClearColor(0, 0, 0, 1);
    glClearDepth(gl_reversez ? 0.0f : 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, c->width, c->height);

    perspective44(proj, 90.0f, c->width / (float)c->height, 0.1f, 1000.f);
    lookat44(view, c->pos, add3(c->pos, directions[c->step]), up_vectors[c->step]);
    ++c->step;

    return true;
}

void cubemap_endbake(cubemap_t *c, int step, float sky_intensity) {
    if (!sky_intensity) {
        sky_intensity = 1.0f;
    }
    if (!step) {
        step = 16;
    }

    if (c->id) {
        glDeleteTextures(1, &c->id);
        c->id = 0;
    }
  
    #if 0
    static unsigned sh_shader = -1, sh_buffer = -1, wg_buffer = -1, u_intensity = -1, u_size = -1, u_face_index = -1, u_texture = -1, u_step = -1, u_pass = -1;
    do_once {
        sh_shader = compute(file_read("shaders/cubemap_sh.gl"));
        glGenBuffers(1, &sh_buffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, sh_buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, 9 * sizeof(vec3), NULL, GL_DYNAMIC_COPY);
        
        u_texture = glGetUniformLocation(sh_shader, "cubeFace");
        u_intensity = glGetUniformLocation(sh_shader, "skyIntensity");
        u_size = glGetUniformLocation(sh_shader, "textureSize");
        u_face_index = glGetUniformLocation(sh_shader, "faceIndex");
        u_step = glGetUniformLocation(sh_shader, "step");
        u_pass = glGetUniformLocation(sh_shader, "pass");
    }

    // Prepare work group buffer
    glGenBuffers(1, &wg_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, wg_buffer);
    int num_work_groups = ((c->width + 15) / 16) * ((c->height + 15) / 16);
    glBufferData(GL_SHADER_STORAGE_BUFFER, num_work_groups * 9 * sizeof(vec3), NULL, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, wg_buffer);
    
    // Clear SH buffer
    vec3 zero = vec3(0,0,0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, sh_buffer);
    glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_RGB32F, GL_RGB, GL_FLOAT, &zero);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, sh_buffer);

    // Set up render parameters
    int step = 16;
    shader_bind(sh_shader);
    glUniform1f(u_intensity, sky_intensity);
    glUniform2i(u_size, c->width, c->height);

    for (int i = 0; i < 6; i++) {
        // Bind texture to texture unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, c->textures[i]);
        glUniform1i(u_texture, 0);

        // Set up face index
        glUniform1i(u_face_index, i);

        // Dispatch compute shader
        glUniform1i(u_pass, 0);
        glDispatchCompute((c->width+step-1)/step, (c->height+step-1)/step, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glUniform1i(u_pass, 1);
        glDispatchCompute((c->width+step-1)/step, (c->height+step-1)/step, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    // Copy SH coefficients from buffer to array
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 9 * sizeof(vec3), c->sh);

    // Normalize SH coefficients
    int total_samples = 16 * 2 * 6;
    for (int s = 0; s < 9; s++) {
        c->sh[s] = scale3(c->sh[s], 32.f / total_samples);
        // c->sh[s] = scale3(c->sh[s], 4.f * M_PI / total_samples);
    }

    glDeleteBuffers(1, &wg_buffer);

    // Generate cubemap texture
    glGenTextures(1, &c->id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, c->id);

    // Copy each face of the cubemap to the cubemap texture
    for (int i = 0; i < 6; ++i) {
        glCopyImageSubData(c->textures[i], GL_TEXTURE_2D, 0, 0, 0, 0,
                           c->id, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, 0,
                           c->width, c->height, 1);
    }
    
    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    #else

    glGenTextures(1, &c->id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, c->id);

    int samples = 0;
    for (int i = 0; i < 6; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, c->framebuffers[i]);
        glReadPixels(0, 0, c->width, c->height, GL_RGB, GL_FLOAT, c->pixels);
  
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, c->width, c->height, 0, GL_RGB, GL_FLOAT, c->pixels);

        // calculate SH coefficients (@ands)
        // copied from cubemap6 method
        const vec3 skyDir[] = {{ 1, 0, 0},{-1, 0, 0},{ 0, 1, 0},{ 0,-1, 0},{ 0, 0, 1},{ 0, 0,-1}};
        const vec3 skyX[]   = {{ 0, 0,-1},{ 0, 0, 1},{ 1, 0, 0},{ 1, 0, 0},{ 1, 0, 0},{-1, 0, 0}};
        // const vec3 skyY[]   = {{ 0, 1, 0},{ 0, 1, 0},{ 0, 0,-1},{ 0, 0, 1},{ 0, 1, 0},{ 0, 1, 0}};
        static vec3 skyY[6] = {{ 0,-1, 0},{ 0,-1, 0},{ 0, 0, 1},{ 0, 0,-1},{ 0,-1, 0},{ 0,-1, 0}};

        for (int y = 0; y < c->height; y += step) {
            float *p = (float*)(c->pixels + y * c->width * 3);
            for (int x = 0; x < c->width; x += step) {
                vec3 n = add3(
                    add3(
                        scale3(skyX[i],  2.0f * (x / (c->width - 1.0f)) - 1.0f),
                        scale3(skyY[i], -2.0f * (y / (c->height - 1.0f)) + 1.0f)),
                    skyDir[i]); // texelDirection;
                float l = len3(n);
                vec3 light = scale3(vec3(p[0], p[1], p[2]), (1 / (l * l * l)) * sky_intensity); // texelSolidAngle * texel_radiance;
                n = norm3(n);
                c->sh[0] = add3(c->sh[0], scale3(light,  0.282095f));
                c->sh[1] = add3(c->sh[1], scale3(light, -0.488603f * n.y * 2.0 / 3.0));
                c->sh[2] = add3(c->sh[2], scale3(light,  0.488603f * n.z * 2.0 / 3.0));
                c->sh[3] = add3(c->sh[3], scale3(light, -0.488603f * n.x * 2.0 / 3.0));
                c->sh[4] = add3(c->sh[4], scale3(light,  1.092548f * n.x * n.y / 4.0));
                c->sh[5] = add3(c->sh[5], scale3(light, -1.092548f * n.y * n.z / 4.0));
                c->sh[6] = add3(c->sh[6], scale3(light,  0.315392f * (3.0f * n.z * n.z - 1.0f) / 4.0));
                c->sh[7] = add3(c->sh[7], scale3(light, -1.092548f * n.x * n.z / 4.0));
                c->sh[8] = add3(c->sh[8], scale3(light,  0.546274f * (n.x * n.x - n.y * n.y) / 4.0));
                p += 3 * step;
                samples++;
            }
        }
    }

    for (int s = 0; s < 9; s++) {
        c->sh[s] = scale3(c->sh[s], 32.f / samples);
    }
    
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    #endif

    glBindFramebuffer(GL_FRAMEBUFFER, sky_last_fb);
    glViewport(sky_last_vp[0], sky_last_vp[1], sky_last_vp[2], sky_last_vp[3]);
}

void cubemap_sh_reset(cubemap_t *c) {
    for (int s = 0; s < 9; s++) {
        c->sh[s] = vec3(0,0,0);
    }
}

void cubemap_sh_addlight(cubemap_t *c, vec3 light, vec3 dir, float strength) {
    // Normalize the direction
    vec3 norm_dir = norm3(dir);

    // Scale the light color and intensity
    vec3 scaled_light = scale3(light, strength);

    // Add light to the SH coefficients
    c->sh[0] = add3(c->sh[0], scale3(scaled_light,  0.282095f));
    c->sh[1] = add3(c->sh[1], scale3(scaled_light, -0.488603f * norm_dir.y));
    c->sh[2] = add3(c->sh[2], scale3(scaled_light,  0.488603f * norm_dir.z));
    c->sh[3] = add3(c->sh[3], scale3(scaled_light, -0.488603f * norm_dir.x));
}

void cubemap_sh_blend(vec3 pos, float max_dist, unsigned count, cubemap_t *probes, vec3 out_sh[9]) {
    if (count == 0) {
        memset(out_sh, 0, 9 * sizeof(vec3));
        return;
    }

    float total_weight = 0.0f;
    vec3 final_sh[9] = {0};

    // Iterate through each probe
    for (unsigned i = 0; i < count; i++) {
        float distance = len3(sub3(pos, probes[i].pos));
        float weight = 1.0f - (distance / max_dist);
        weight = weight * weight;

        for (int s = 0; s < 9; s++) {
            final_sh[s] = add3(final_sh[s], scale3(probes[i].sh[s], weight));
        }

        total_weight += weight;
    }

    // Normalize the final SH coefficients
    for (int s = 0; s < 9; s++) {
        final_sh[s] = scale3(final_sh[s], 1.0f / total_weight);
    }

    // Apply SH coefficients to the shader
    memcpy(out_sh, final_sh, 9 * sizeof(vec3));
}

#endif
