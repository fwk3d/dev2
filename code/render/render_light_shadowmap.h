// -----------------------------------------------------------------------------
// shadowmaps

#if !CODE

typedef struct shadowmap_t {
    mat44 V;
    mat44 PV;
    int vsm_texture_width;
    int csm_texture_width;
    int step;
    int light_step;
    int cascade_index;
    int max_cascades; //< can not be bigger than NUM_SHADOW_CASCADES
    unsigned shadow_technique;
    float cascade_splits[NUM_SHADOW_CASCADES];
    frustum shadow_frustum;

    // signals
    bool skip_render;
    int lights_pushed;
    handle fbo;

    // VRAM usage
    uint64_t vram_usage;
    uint64_t vram_usage_total;
    uint64_t vram_usage_vsm;
    uint64_t vram_usage_csm;

    // depth texture
    handle depth_texture;
    handle depth_texture_2d;

    // cascaded shadowmap blending
    float blend_region;

    // shadowmap offsets texture;
    int filter_size, window_size;
    handle offsets_texture;

    struct {
        int gen;
        unsigned shadow_technique;
        handle texture;
        handle texture_2d[NUM_SHADOW_CASCADES];
        float cascade_distances[NUM_SHADOW_CASCADES];
    } maps[MAX_SHADOW_LIGHTS];

    handle saved_fb;
    handle saved_pass;
    int saved_vp[4];
    int gen;
    int old_filter_size;
    int old_window_size;
} shadowmap_t;

API shadowmap_t shadowmap(int vsm_texture_width, int csm_texture_width); // = 512, 4096
API void          shadowmap_offsets_build(shadowmap_t *s, int filter_size, int window_size);
API void        shadowmap_destroy(shadowmap_t *s);

API void shadowmap_begin(shadowmap_t *s);
API bool   shadowmap_step(shadowmap_t *s); //< roll over to the next light if it returns false
API void     shadowmap_light(shadowmap_t *s, light_t *l, mat44 cam_proj, mat44 cam_view); //< must be called at most once per shadowmap_step
API void shadowmap_end(shadowmap_t *s);
API void ui_shadowmap(shadowmap_t *s);

#else

// -----------------------------------------------------------------------------
// shadowmaps

static inline
void shadowmap_init_common_resources(shadowmap_t *s, int vsm_texture_width, int csm_texture_width) {
    // Create a cubemap depth texture for Variance Shadow Mapping (VSM)
    glGenTextures(1, &s->depth_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, s->depth_texture);
    for (int i = 0; i < 6; i++) {
        // Create a 16-bit depth component texture for each face of the cubemap
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT16, vsm_texture_width, vsm_texture_width, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, 0);
    }

    // Unbind the cubemap texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // Create a 2D depth texture for Cascaded Shadow Mapping (CSM)
    glGenTextures(1, &s->depth_texture_2d);
    glBindTexture(GL_TEXTURE_2D, s->depth_texture_2d);
    // Create a single 16-bit depth component texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, csm_texture_width, csm_texture_width, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, 0);

    // Unbind the 2D texture
    glBindTexture(GL_TEXTURE_2D, 0);
}


#if is(ems) // @todo ems support

shadowmap_t shadowmap(int vsm_texture_width, int csm_texture_width) { // = 512, 4096
    shadowmap_t s = {0};
    s.vsm_texture_width = vsm_texture_width;
    s.csm_texture_width = csm_texture_width;
    s.saved_fb = 0;
    s.filter_size = 4;
    s.window_size = 4;
    s.cascade_splits[0] = 0.1f;
    s.cascade_splits[1] = 0.5f;
    s.cascade_splits[2] = 1.0f;
    s.cascade_splits[3] = 1.0f;  /* sticks to camera far plane */
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &s.saved_fb);
    glBindFramebuffer(GL_FRAMEBUFFER, s.saved_fb);
    return s;
}

static inline
void shadowmap_destroy_light(shadowmap_t *s, int light_index) {
    s->maps[light_index].gen = 0;
    s->maps[light_index].shadow_technique = 0xFFFF;
}

void shadowmap_destroy(shadowmap_t *s) {
    for (int i = 0; i < MAX_LIGHTS; i++) {
        shadowmap_destroy_light(s, i);
    }
    shadowmap_t z = {0};
    *s = z;
}

static shadowmap_t *active_shadowmap = NULL;

void shadowmap_begin(shadowmap_t *s) {
    glGetIntegerv(GL_VIEWPORT, s->saved_vp);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &s->saved_fb);

    for (int i = 0; i < MAX_SHADOW_LIGHTS; i++) {
        if (s->maps[i].gen != s->gen) {
            shadowmap_destroy_light(s, i);
        }
    }

    s->step = 0;
    s->light_step = 0;
    s->cascade_index = 0;
    s->gen++;
    active_shadowmap = s;
}

bool shadowmap_step(shadowmap_t *s) {
    s->skip_render = true;
    return false;
}

void shadowmap_light(shadowmap_t *s, light_t *l, mat44 cam_proj, mat44 cam_view) {
    l->processed_shadows = false;
}

void shadowmap_end(shadowmap_t *s) {
}

void ui_shadowmap(shadowmap_t *s) {
}

#else

static inline void
shadowmap_init_caster_vsm(shadowmap_t *s, int light_index, int texture_width) {
    float borderColor[] = {1.0, 1.0, 1.0, 1.0};

    if (s->maps[light_index].texture) {
        return;
    }

    // Create a cubemap moments texture
    glGenTextures(1, &s->maps[light_index].texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, s->maps[light_index].texture);
    for (int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RG32F, texture_width, texture_width, 0, GL_RG, GL_FLOAT, 0);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

static inline void
shadowmap_init_caster_csm(shadowmap_t *s, int light_index, int texture_width) {
    float borderColor[] = {1.0, 1.0, 1.0, 1.0};

    if (s->maps[light_index].texture_2d[0]) {
        return;
    }

    // Initialise shadow map 2D
    for (int i = 0; i < NUM_SHADOW_CASCADES; i++) {
        int tw = texture_width>>i;
        glGenTextures(1, &s->maps[light_index].texture_2d[i]);
        glBindTexture(GL_TEXTURE_2D, s->maps[light_index].texture_2d[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, tw, tw, 0, GL_RED, GL_HALF_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

shadowmap_t shadowmap(int vsm_texture_width, int csm_texture_width) { // = 512, 4096
    shadowmap_t s = {0};
    s.vsm_texture_width = vsm_texture_width;
    s.csm_texture_width = csm_texture_width;
    s.saved_fb = 0;
    s.filter_size = 4;
    s.window_size = 8;
    s.blend_region = 15.0f;

    s.max_cascades = NUM_SHADOW_CASCADES;
    for (int i = 0; i < NUM_SHADOW_CASCADES; i++) {
        float t = (float)i / (NUM_SHADOW_CASCADES - 1);
        s.cascade_splits[i] = expf(-2.3f * (1.0f - t));
    }

    glGenFramebuffers(1, &s.fbo);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &s.saved_fb);

    for (int i = 0; i < MAX_SHADOW_LIGHTS; i++) {
        s.maps[i].shadow_technique = 0xFFFF;
        for (int j = 0; j < NUM_SHADOW_CASCADES; j++) {
            s.maps[i].cascade_distances[j] = 0.0f;
        }
    }

    shadowmap_init_common_resources(&s, vsm_texture_width, csm_texture_width);

    glBindFramebuffer(GL_FRAMEBUFFER, s.saved_fb);
    return s;
}

static inline
float shadowmap_offsets_build_jitter() {
    return (randf() - 0.5f);
    // return ease_inout_perlin(randf()) - 0.5f;
}

static inline
float *shadowmap_offsets_build_data(int filter_size, int window_size) {
    int bufsize = filter_size * filter_size * window_size * window_size * 2;
    float *data = MALLOC(bufsize * sizeof(float));

    int index = 0;

    for (int y = 0; y < window_size; y++) {
        for (int x = 0; x < window_size; x++) {
            for (int v = filter_size-1; v >= 0; v--) {
                for (int u = 0; u < filter_size; u++) {
                    float x = ((float)(u + 0.5f + shadowmap_offsets_build_jitter()) / (float)filter_size);
                    float y = ((float)(v + 0.5f + shadowmap_offsets_build_jitter()) / (float)filter_size);
                    ASSERT(index + 1 < bufsize);
                    data[index+0] = sqrtf(y) * cosf(2 * M_PI * x);
                    data[index+1] = sqrtf(y) * sinf(2 * M_PI * x);
                    index += 2;
                }
            }
        }
    }
    return data;
}

void shadowmap_offsets_build(shadowmap_t *s, int filter_size, int window_size) {
    if (s->offsets_texture) {
        glDeleteTextures(1, &s->offsets_texture);
        s->offsets_texture = 0;
    }

    s->filter_size = filter_size;
    s->window_size = window_size;
    int num_samples = filter_size * filter_size;

    float *data = shadowmap_offsets_build_data(filter_size, window_size);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &s->offsets_texture);
    glBindTexture(GL_TEXTURE_3D, s->offsets_texture);
    glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA32F, num_samples / 2, window_size, window_size);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, num_samples / 2, window_size, window_size, GL_RGBA, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_3D, 0);

    FREE(data);
}

static inline
void shadowmap_destroy_light(shadowmap_t *s, int light_index) {
    s->maps[light_index].gen = 0;
    s->maps[light_index].shadow_technique = 0xFFFF;

    if (s->maps[light_index].texture) {
        glDeleteTextures(1, &s->maps[light_index].texture);
        s->maps[light_index].texture = 0;
    }
    
    for (int i = 0; i < NUM_SHADOW_CASCADES; i++) {
        if (s->maps[light_index].texture_2d[i]) {
            glDeleteTextures(1, &s->maps[light_index].texture_2d[i]);
            s->maps[light_index].texture_2d[i] = 0;
        }
    }
}

void shadowmap_destroy(shadowmap_t *s) {
    for (int i = 0; i < MAX_SHADOW_LIGHTS; i++) {
        shadowmap_destroy_light(s, i);
    }

    if (s->depth_texture) {
        glDeleteTextures(1, &s->depth_texture);
        s->depth_texture = 0;
    }

    if (s->depth_texture_2d) {
        glDeleteTextures(1, &s->depth_texture_2d);
        s->depth_texture_2d = 0;
    }

    shadowmap_t z = {0};
    *s = z;
}

static shadowmap_t *active_shadowmap = NULL;

void shadowmap_begin(shadowmap_t *s) {
    glGetIntegerv(GL_VIEWPORT, s->saved_vp);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &s->saved_fb);
    s->max_cascades = clampi(s->max_cascades, 0, NUM_SHADOW_CASCADES);

    for (int i = 0; i < MAX_SHADOW_LIGHTS; i++) {
        if (s->maps[i].gen != s->gen) {
            shadowmap_destroy_light(s, i);
        }
    }

    if (s->filter_size != s->old_filter_size || s->window_size != s->old_window_size) {
        shadowmap_offsets_build(s, s->filter_size, s->window_size);
        s->old_filter_size = s->filter_size;
        s->old_window_size = s->window_size;
    }

    s->step = 0;
    s->light_step = 0;
    s->cascade_index = 0;
    s->gen++;
    active_shadowmap = s;
}

static void shadowmap_light_point(shadowmap_t *s, light_t *l, float cam_far, int dir) {
    if(dir<0) return;

    float shadow_distance = l->shadow_distance;
    if (shadow_distance == 0.0f) {
        shadow_distance = cam_far;
    }

    mat44 P, V, PV;
    perspective44(P, 90.0f, 1.0f, l->shadow_near_clip, shadow_distance);
    vec3 lightPos = l->pos;

    /**/ if(dir == 0) lookat44(V, lightPos, add3(lightPos, vec3(+1,  0,  0)), vec3(0, -1,  0)); // +X
    else if(dir == 1) lookat44(V, lightPos, add3(lightPos, vec3(-1,  0,  0)), vec3(0, -1,  0)); // -X
    else if(dir == 2) lookat44(V, lightPos, add3(lightPos, vec3( 0, +1,  0)), vec3(0,  0, +1)); // +Y
    else if(dir == 3) lookat44(V, lightPos, add3(lightPos, vec3( 0, -1,  0)), vec3(0,  0, -1)); // -Y
    else if(dir == 4) lookat44(V, lightPos, add3(lightPos, vec3( 0,  0, +1)), vec3(0, -1,  0)); // +Z
    else /*dir == 5*/ lookat44(V, lightPos, add3(lightPos, vec3( 0,  0, -1)), vec3(0, -1,  0)); // -Z
    multiply44x2(PV, P, V); // -Z

    copy44(s->V, V);
    copy44(s->PV, PV);
    
    l->processed_shadows = true;
    s->shadow_technique = l->shadow_technique = SHADOW_VSM;
}

static array(vec3) frustum_corners = 0;

static inline
void shadowmap_light_directional_calc_frustum_corners(mat44 cam_proj, mat44 cam_view) {
    mat44 PV; multiply44x2(PV, cam_proj, cam_view);
    mat44 inverse_view_proj; invert44(inverse_view_proj, PV);
    array_resize(frustum_corners, 0);
    for (unsigned x = 0; x < 2; x++) {
        for (unsigned y = 0; y < 2; y++) {
            for (unsigned z = 0; z < 2; z++) {
                vec4 corner = {
                    x * 2.0f - 1.0f,
                    y * 2.0f - 1.0f,
                    z * 2.0f - 1.0f,
                    1.0f
                };
                vec4 world_corner = transform444(inverse_view_proj, corner);
                world_corner = scale4(world_corner, 1.0f / world_corner.w);
                array_push(frustum_corners, vec3(world_corner.x, world_corner.y, world_corner.z));
            }
        }
    }
}

static void shadowmap_light_directional(shadowmap_t *s, light_t *l, int dir, float cam_fov, float cam_far, mat44 cam_view) {
    if (dir != 0) {
        s->skip_render = true;
        return;
    }

    float far_plane = 0.0f;
    float near_plane = 0.0f;

    float shadow_distance = l->shadow_distance;
    if (shadow_distance == 0.0f) {
        shadow_distance = cam_far;
    }

    if (s->cascade_index == 0 && s->max_cascades > 1) {
        near_plane = l->shadow_near_clip;
        far_plane = shadow_distance * s->cascade_splits[0];
    } else if (s->cascade_index < s->max_cascades - 1) {
        near_plane = shadow_distance * s->cascade_splits[s->cascade_index-1]*SHADOW_CASCADE_BLEND_REGION;
        far_plane = shadow_distance * s->cascade_splits[s->cascade_index];
    } else {
        near_plane = shadow_distance * s->cascade_splits[s->max_cascades-1]*SHADOW_CASCADE_BLEND_REGION;
        far_plane = shadow_distance == 0.0f ? cam_far : shadow_distance;
    }

    mat44 proj; 
    // perspective44(proj, 75, 1.0f, near_plane, far_plane);
    perspective44(proj, cam_fov, window_width()/(float)window_height(), near_plane, far_plane);
    shadowmap_light_directional_calc_frustum_corners(proj, cam_view);
    vec3 center = {0,0,0};
    float sphere_radius = 0.0f;
    for (unsigned i = 0; i < array_count(frustum_corners); i++) {
        center = add3(center, frustum_corners[i]);
        float dist = len3(frustum_corners[i]);
        sphere_radius = max(sphere_radius, dist);
    }
    center = scale3(center, 1.0f / array_count(frustum_corners));


    s->maps[s->light_step].cascade_distances[s->cascade_index] = far_plane;

    float minX = FLT_MAX, maxX = FLT_MIN;
    float minY = FLT_MAX, maxY = FLT_MIN;
    float minZ = FLT_MAX, maxZ = FLT_MIN;

    mat44 V;
    vec3 lightDir = norm3(l->dir);
    vec3 up = vec3(0, 1, 0);

    lookat44(V, sub3(center, scale3(lightDir, sphere_radius)), add3(center, scale3(lightDir, sphere_radius)), up);

    for (unsigned i = 0; i < array_count(frustum_corners); i++) {
        vec3 corner = frustum_corners[i];

        corner = transform344(V, corner);
        minX = min(minX, corner.x);
        maxX = max(maxX, corner.x);
        minY = min(minY, corner.y);
        maxY = max(maxY, corner.y);
        minZ = min(minZ, corner.z);
        maxZ = max(maxZ, corner.z);
    }

#if 0
    float tmpZ = -minZ;
    minZ = -maxZ;
    maxZ = tmpZ;

    float mid = (maxZ + minZ) * 0.5f;
    minZ -= mid * 5.0f;
    maxZ += mid * 5.0f;
#endif

    mat44 P, PV;
    ortho44(P, 
        minX, maxX, 
        minY, maxY, 
        // minZ, maxZ);
        -maxZ, -minZ);

    multiply44x2(PV, P, V);

    copy44(s->V, V);
    copy44(s->PV, PV);
    copy44(l->shadow_matrix[s->cascade_index], PV);

    l->processed_shadows = true;
    l->cached = 0;
    s->shadow_technique = l->shadow_technique = SHADOW_CSM;
}

static inline
bool shadowmap_step_finish(shadowmap_t *s) {
    if (s->shadow_technique == SHADOW_CSM) {
        if (s->cascade_index < s->max_cascades-1) {
            s->cascade_index++;
            s->step = 0;
            return false;
        }
    }

    s->step = 0;
    s->light_step++;
    s->cascade_index = 0;
    return true;
}

bool shadowmap_step(shadowmap_t *s) {
    int max_steps = s->shadow_technique == 0xffff ? 1 : s->shadow_technique == SHADOW_CSM ? 1 : 6;
    if (s->step >= max_steps) {
        return !shadowmap_step_finish(s);
    }

    if (s->light_step >= MAX_SHADOW_LIGHTS) {
        return false;
    }

    s->step++;
    s->skip_render = false;
    s->lights_pushed = 0;
    return true;
}

static inline
void shadowmap_clear_fbo() {
    glClearColor(1, 1, 1, 1);
    glClearDepth(gl_reversez ? 0.0f : 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void shadowmap_light(shadowmap_t *s, light_t *l, mat44 cam_proj, mat44 cam_view) {
    l->processed_shadows = false;
    if (l->cast_shadows) {
        int step = s->step - 1;

        float y_scale = cam_proj[5];
        float cam_fov = (2.0f * atan(1.0f / y_scale)) * TO_DEG;
        float cam_far = 0.0f; {
            float m22 = cam_proj[10];
            float m32 = cam_proj[14];
            float near_plane = -m32 / (m22 + 1.0f);
            cam_far = (2.0f * near_plane) / (m22 - 1.0f);
            cam_far *= 0.5f;
        }

        if (l->type == LIGHT_POINT || l->type == LIGHT_SPOT) {
            shadowmap_light_point(s, l, cam_far, step);
        } else if (l->type == LIGHT_DIRECTIONAL) {
            shadowmap_light_directional(s, l, step, cam_fov, cam_far, cam_view);
        }

        if (s->skip_render) {
            return;
        }

        if (s->maps[s->light_step].shadow_technique != l->shadow_technique) {
            shadowmap_destroy_light(s, s->light_step);
            if (l->shadow_technique == SHADOW_VSM) {
                shadowmap_init_caster_vsm(s, s->light_step, s->vsm_texture_width);
            } else if (l->shadow_technique == SHADOW_CSM) {
                shadowmap_init_caster_csm(s, s->light_step, s->csm_texture_width);
            }
        }

        s->maps[s->light_step].gen = s->gen;
        s->maps[s->light_step].shadow_technique = l->shadow_technique;

        ASSERT(s->lights_pushed == 0);
        s->lights_pushed++;

        if (l->type == LIGHT_DIRECTIONAL) {
            glBindFramebuffer(GL_FRAMEBUFFER, s->fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s->maps[s->light_step].texture_2d[s->cascade_index], 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, s->depth_texture_2d, 0);
            shadowmap_clear_fbo();
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, s->fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + step, s->maps[s->light_step].texture, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + step, s->depth_texture, 0);
            shadowmap_clear_fbo();
        }
    
        unsigned texture_width = s->shadow_technique == SHADOW_VSM ? s->vsm_texture_width : s->csm_texture_width;
        if (s->shadow_technique == SHADOW_CSM) {
            texture_width >>= s->cascade_index;
        }
        glViewport(0, 0, texture_width, texture_width);

        s->shadow_frustum = frustum_build(s->PV);
    } else {
        s->skip_render = true;
    }
}

void shadowmap_end(shadowmap_t *s) {
    glViewport(s->saved_vp[0], s->saved_vp[1], s->saved_vp[2], s->saved_vp[3]);
    glBindFramebuffer(GL_FRAMEBUFFER, s->saved_fb);
    active_shadowmap = NULL;

    // calculate vram usage
    s->vram_usage = 0;
    s->vram_usage_total = 0;
    s->vram_usage_vsm = 0;
    s->vram_usage_csm = 0;
    {
        // Common resources
        s->vram_usage += 6 * s->vsm_texture_width * s->vsm_texture_width * 2; // VSM depth texture (GL_DEPTH_COMPONENT16)
        s->vram_usage += s->csm_texture_width * s->csm_texture_width * 2; // CSM depth texture (GL_DEPTH_COMPONENT16)

        // Per-light resources
        for (int i = 0; i < MAX_SHADOW_LIGHTS; i++) {
            if (s->maps[i].shadow_technique == SHADOW_VSM) {
                // VSM cubemap texture (GL_RGB32F)
                s->vram_usage_vsm += 6 * s->vsm_texture_width * s->vsm_texture_width * 8;
            } else if (s->maps[i].shadow_technique == SHADOW_CSM) {
                // CSM textures (GL_RG16F)
                s->vram_usage_csm += NUM_SHADOW_CASCADES * s->csm_texture_width * s->csm_texture_width * 2;
            }
        }
        s->vram_usage_total = s->vram_usage + s->vram_usage_vsm + s->vram_usage_csm;
    }
}

void ui_shadowmap(shadowmap_t *s) {
    if (!s) return;

    int vsm_width = s->vsm_texture_width;
    int csm_width = s->csm_texture_width;
    ui_int("Texture Width (VSM)", &vsm_width);
    ui_int("Texture Width (CSM)", &csm_width);
    ui_int("Max Cascades", &s->max_cascades);
    ui_float("Blend Region", &s->blend_region);

    if (ui_collapse("Shadowmap Offsets", "shadowmap_offsets")) {
        ui_int("Filter Size", &s->filter_size);
        ui_int("Window Size", &s->window_size);
        ui_collapse_end();
    }

    if (ui_collapse("VRAM Usage", "vram_usage")) {
        ui_label2("Total VRAM", va("%lld KB", s->vram_usage_total / 1024));
        ui_label2("VSM VRAM", va("%lld KB", s->vram_usage_vsm / 1024));
        ui_label2("CSM VRAM", va("%lld KB", s->vram_usage_csm / 1024));
        ui_label2("Depth Texture VRAM", va("%lld KB", s->vram_usage / 1024));
        ui_collapse_end();
    }
}

#endif // is(ems)

#endif // CODE
