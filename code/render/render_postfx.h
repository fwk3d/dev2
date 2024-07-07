// -----------------------------------------------------------------------------
// post-fxs swapchain

#if !CODE

API int      fx_load(const char *file);
API int      fx_load_from_mem(const char *nameid, const char *content);
API bool     fx_begin();
API bool     fx_begin_res(int w, int h);
API bool     fx_end(unsigned texture_id, unsigned depth_id);
API void     fx_apply(texture_t color_texture, texture_t depth_texture);
API void     fx_enable(int pass, int enabled);
API int      fx_enable_ordered(int pass);
API int      fx_enabled(int pass);
API void     fx_enable_all(int enabled);
API char *   fx_name(int pass);
API int      fx_find(const char *name);
API void     fx_setparam(int pass, const char *name, float value);
API void     fx_setparami(int pass, const char *name, int value);
API void     fx_setparam3(int pass, const char *name, vec3 value);
API void     fx_setparam4(int pass, const char *name, vec4 value);
API void     fx_setparamt(int pass, const char *name, texture_t value, int unit);
API int      fx_order(int pass, unsigned priority);
API unsigned fx_program(int pass);

API int      ui_fx(int pass);
API int      ui_fxs();

// single-pass draw
API void fx_drawpass_rs(int pass, texture_t color, texture_t depth, renderstate_t *rs);
API void fx_drawpass(int pass, texture_t color, texture_t depth);

// low-level API

typedef struct passfx {
    char *name;
    unsigned program;
    int uniforms[16];
    unsigned priority;
    bool enabled;
} passfx;

typedef struct postfx {
    unsigned vao;
    // renderbuffers: color & depth textures
    unsigned fb[2];
    texture_t diffuse[2], depth[2];
    // shader passes
    array(passfx) pass;
    // global enable flag
    bool enabled;
    // rolling pass id
    unsigned rolling_id;
    set(char*) added;
} postfx;

API void postfx_create(postfx *fx, int flags);
API void postfx_destroy(postfx *fx);
API bool postfx_load(postfx *fx, const char *filemask);
API bool postfx_begin(postfx *fx, int width, int height);
API bool postfx_end(postfx *fx, unsigned texture_id, unsigned depth_id);
API bool postfx_apply(postfx *fx, texture_t color_texture, texture_t depth_texture);
API bool postfx_enabled(postfx *fx, int pass_number);
API bool postfx_enable(postfx *fx, int pass_number, bool enabled);
API int postfx_enable_ordered(postfx *fx, int pass);
API void postfx_enable_all(postfx *fx, bool enabled);
API void postfx_clear(postfx *fx);
API int postfx_order(postfx *fx, int pass, unsigned priority);
API unsigned postfx_program(postfx *fx, int pass);
API void postfx_setparam(postfx *fx, int pass, const char *name, float value);
API void postfx_setparami(postfx *fx, int pass, const char *name, int value);
API void postfx_setparam3(postfx *fx, int pass, const char *name, vec3 value);
API void postfx_setparam4(postfx *fx, int pass, const char *name, vec4 value);
API void postfx_setparamt(postfx *fx, int pass, const char *name, texture_t value, int unit);
API char* postfx_name(postfx *fx, int slot);
API passfx* postfx_pass(postfx *fx, int slot);

// single-pass draw
API void postfx_drawpass_rs(postfx *fx, int pass, texture_t color, texture_t depth, renderstate_t *rs);
API void postfx_drawpass(postfx *fx, int pass, texture_t color, texture_t depth);

API int   ui_postfx(postfx *fx, int slot);
API int   ui_postfxs(postfx *fx);

// multi-pass fx techniques

typedef struct bloom_params_t {
    int mips_count; // defaults: 4
    float filter_radius; // defaults: 0.005
    float strength; // defaults: 0.80
    float threshold; // defaults: 0.00
    float soft_threshold; // defaults: 0.00
    bool suppress_fireflies; // defaults: true
} bloom_params_t;

API texture_t fxt_bloom(texture_t color, bloom_params_t params);

typedef struct reflect_params_t {
    bool disabled; // defaults: false
    float max_distance; // defaults: 100.0
    float reflection_strength; // defaults: 0.5
    float metallic_threshold; // defaults: 0.001
    int downsample; // defaults: 0
    cubemap_t *cubemap; // defaults: NULL
} reflect_params_t;

API texture_t fxt_reflect(texture_t color, texture_t depth, texture_t normal, texture_t matprops, mat44 proj, mat44 view, reflect_params_t params);

#else

enum {
    u_color,
    u_depth,
    u_time,
    u_frame,
    u_width, u_height,
    u_mousex, u_mousey, 
    u_mousez, u_mousew,
    u_channelres0x, u_channelres0y,
    u_channelres1x, u_channelres1y,
};

void postfx_create(postfx *fx, int flags) {
    postfx z = {0};
    *fx = z;
    fx->enabled = 1;
    glGenVertexArrays(1, &fx->vao);
    set_init_str(fx->added);
    (void)flags;
}

void postfx_destroy( postfx *fx ) {
    for( int i = 0; i < array_count(fx->pass); ++i ) {
        FREE(fx->pass[i].name);
    }
    glDeleteVertexArrays(1, &fx->vao);
    array_free(fx->pass);
    texture_destroy(&fx->diffuse[0]);
    texture_destroy(&fx->diffuse[1]);
    texture_destroy(&fx->depth[0]);
    texture_destroy(&fx->depth[1]);
    fbo_destroy_id(fx->fb[0]);
    fbo_destroy_id(fx->fb[1]);
    postfx z = {0};
    *fx = z;
}

char* postfx_name(postfx *fx, int slot) {
    return slot < 0 || slot >= array_count(fx->pass) ? "" : fx->pass[ slot ].name;
}
passfx* postfx_pass(postfx *fx, int slot) {
    return slot < 0 || slot >= array_count(fx->pass) ? NULL : &fx->pass[slot];
}
int postfx_find(postfx *fx, const char *name) {
    name = file_name(name);
    for( int i = 0; i < array_count(fx->pass); ++i) if(!strcmpi(fx->pass[i].name, name)) return i;
    return -1;
}

static
int postfx_sort_fn(const void *a, const void *b) {
    unsigned p1 = ((passfx*)a)->priority;
    unsigned p2 = ((passfx*)b)->priority;
    return (p1 > p2) - (p1 < p2);
}
int postfx_order(postfx *fx, int pass, unsigned priority) {
    if (pass < 0 || pass >= array_count(fx->pass)) return -1;
    if (priority >= array_count(fx->pass)) return -1;
    fx->pass[priority].priority = pass;
    fx->pass[pass].priority = priority;
    int prog = fx->pass[pass].program;
    array_sort(fx->pass, postfx_sort_fn);
    int new_pass = -1;
    for (int i = 0; i < array_count(fx->pass); i++) {
        if (fx->pass[i].program == prog) {
            new_pass = i;
            break;
        }
    }
    return new_pass;
}

int postfx_load_from_mem( postfx *fx, const char *name, const char *fs ) {
    PRINTF("%s\n", name);
    if(!fs || !fs[0]) return -1; // die("!invalid fragment shader");

    passfx pass={0};
    array_push(fx->pass, pass);
    passfx *p = array_back(fx->pass);
    p->name = STRDUP(name);
    p->priority = array_count(fx->pass)-1;

    // preload stuff
    static const char *vs = 0;
    static const char *preamble = 0;
    static const char *shadertoy = 0;
    static char *fs2 = 0;
    do_once {
        vs = STRDUP(file_read("shaders/fullscreen_quad_B_vs.gl",0));
        preamble = STRDUP(file_read("shaders/preamble_fs.gl",0));
        shadertoy = STRDUP(file_read("shaders/shadertoy_main_fs.gl",0));
        fs2 = (char*)CALLOC(1, 128*1024);
    }
    // patch fragment
    snprintf(fs2, 128*1024, "%s%s%s", preamble, strstr(fs, "mainImage") ? shadertoy : "", fs );

    p->program = shader(vs, fs2, "vtexcoord", "fragColor" , NULL);

    glUseProgram(p->program); // needed?

    for( int i = 0; i < countof(p->uniforms); ++i ) p->uniforms[i] = -1;

    if( p->uniforms[u_time] == -1 )   p->uniforms[u_time] = glGetUniformLocation(p->program, "iTime");

    if( p->uniforms[u_frame] == -1 )   p->uniforms[u_frame] = glGetUniformLocation(p->program, "iFrame");

    if( p->uniforms[u_width] == -1 )  p->uniforms[u_width] = glGetUniformLocation(p->program, "iWidth");
    if( p->uniforms[u_height] == -1 ) p->uniforms[u_height] = glGetUniformLocation(p->program, "iHeight");

    if( p->uniforms[u_mousex] == -1 ) p->uniforms[u_mousex] = glGetUniformLocation(p->program, "iMousex");
    if( p->uniforms[u_mousey] == -1 ) p->uniforms[u_mousey] = glGetUniformLocation(p->program, "iMousey");
    if( p->uniforms[u_mousez] == -1 ) p->uniforms[u_mousez] = glGetUniformLocation(p->program, "iMousez");
    if( p->uniforms[u_mousew] == -1 ) p->uniforms[u_mousew] = glGetUniformLocation(p->program, "iMousew");

    if( p->uniforms[u_color] == -1 ) p->uniforms[u_color] = glGetUniformLocation(p->program, "tex");
    if( p->uniforms[u_color] == -1 ) p->uniforms[u_color] = glGetUniformLocation(p->program, "tex0");
    if( p->uniforms[u_color] == -1 ) p->uniforms[u_color] = glGetUniformLocation(p->program, "tColor");
    if( p->uniforms[u_color] == -1 ) p->uniforms[u_color] = glGetUniformLocation(p->program, "tDiffuse");
    if( p->uniforms[u_color] == -1 ) p->uniforms[u_color] = glGetUniformLocation(p->program, "iChannel0");

    if( p->uniforms[u_depth] == -1 ) p->uniforms[u_depth] = glGetUniformLocation(p->program, "tex1");
    if( p->uniforms[u_depth] == -1 ) p->uniforms[u_depth] = glGetUniformLocation(p->program, "tDepth");
    if( p->uniforms[u_depth] == -1 ) p->uniforms[u_depth] = glGetUniformLocation(p->program, "iChannel1");

    if( p->uniforms[u_channelres0x] == -1 ) p->uniforms[u_channelres0x] = glGetUniformLocation(p->program, "iChannelRes0x");
    if( p->uniforms[u_channelres0y] == -1 ) p->uniforms[u_channelres0y] = glGetUniformLocation(p->program, "iChannelRes0y");

    if( p->uniforms[u_channelres1x] == -1 ) p->uniforms[u_channelres1x] = glGetUniformLocation(p->program, "iChannelRes1x");
    if( p->uniforms[u_channelres1y] == -1 ) p->uniforms[u_channelres1y] = glGetUniformLocation(p->program, "iChannelRes1y");
    
    return array_count(fx->pass)-1;
}
bool postfx_load( postfx *fx, const char *filemask ) {
    for each_array( file_list(filemask, NULL), char*, list ) {
        if( set_find(fx->added, list) ) continue;
        char *name = STRDUP(list); // @leak
        set_insert(fx->added, name);
        (void)postfx_load_from_mem(fx, file_name(name), file_read(name, 0));
    }
    return true;
}

bool postfx_enable(postfx *fx, int pass, bool enabled) {
    if( pass < 0 || pass >= array_count(fx->pass) ) return false;
    fx->pass[pass].enabled = enabled;
    fx->enabled = !!array_count(fx->pass);
    return fx->enabled;
}

int postfx_enable_ordered(postfx *fx, int pass) {
    postfx_enable(fx, pass, 1);
    return postfx_order(fx, pass, fx->rolling_id++);
}

void postfx_enable_all(postfx *fx, bool enabled) {
    if (!enabled) fx->rolling_id = 0;
    for (int i = 0; i < array_count(fx->pass); i++) {
        fx->pass[i].enabled = enabled;
    }
}

bool postfx_enabled(postfx *fx, int pass) {
    if( pass < 0 || pass >= array_count(fx->pass) ) return false;
    return fx->pass[pass].enabled;
}

bool postfx_toggle(postfx *fx, int pass) {
    if( pass < 0 || pass >= array_count(fx->pass) ) return false;
    return postfx_enable(fx, pass, 1 ^ postfx_enabled(fx, pass));
}

void postfx_clear(postfx *fx) {
    for (int i = 0; i < array_count(fx->pass); i++) {
        fx->pass[i].enabled = 0;
    }
    fx->enabled = 0;
}
unsigned postfx_program(postfx *fx, int pass) {
    if( pass < 0 || pass >= array_count(fx->pass) ) return 0;
    return fx->pass[pass].program;
}
void postfx_setparam(postfx *fx, int pass, const char *name, float value) {
    unsigned program = postfx_program(fx, pass);
    if( !program ) return;
    unsigned oldprogram = shader_bind(program);
    shader_float(name, value);
    shader_bind(oldprogram);
}
void postfx_setparami(postfx *fx, int pass, const char *name, int value) {
    unsigned program = postfx_program(fx, pass);
    if( !program ) return;
    unsigned oldprogram = shader_bind(program);
    shader_int(name, value);
    shader_bind(oldprogram);
}
void postfx_setparam3(postfx *fx, int pass, const char *name, vec3 value) {
    unsigned program = postfx_program(fx, pass);
    if( !program ) return;
    unsigned oldprogram = shader_bind(program);
    shader_vec3(name, value);
    shader_bind(oldprogram);
}
void postfx_setparam4(postfx *fx, int pass, const char *name, vec4 value) {
    unsigned program = postfx_program(fx, pass);
    if( !program ) return;
    unsigned oldprogram = shader_bind(program);
    shader_vec4(name, value);
    shader_bind(oldprogram);
}
void postfx_setparamt(postfx *fx, int pass, const char *name, texture_t value, int unit) {
    unsigned program = postfx_program(fx, pass);
    if( !program ) return;
    unsigned oldprogram = shader_bind(program);
    shader_texture(name, value.id, unit);
    shader_bind(oldprogram);
}
int ui_postfx(postfx *fx, int pass) {
    if (pass < 0 || pass >= array_count(fx->pass)) return 0;
    int on = ui_enabled();
    ui_enable( postfx_enabled(fx,pass) );
    int rc = ui_shader(fx->pass[pass].program);
    ui_separator();
    int btn = ui_buttons(2, "Move up", "Move down");
    if (btn == 1) {
        postfx_order(fx, pass, fx->pass[pass].priority-1);
    } 
    else if (btn == 2) {
        postfx_order(fx, pass, fx->pass[pass].priority+1);
    }
    ui_enable( on );
    return rc;
}
int ui_postfxs(postfx *fx) {
    if(!array_count(fx->pass)) return ui_label(UI_ICON(WARNING) " No Post FXs with annotations loaded."), 0;

    if (ui_button("Active to top")) {
        // Reorder passes so active ones are on top
        int active_count = 0;
        for (int i = 0; i < array_count(fx->pass); ++i) {
            if (fx->pass[i].enabled) {
                if (i != active_count) {
                    passfx temp = fx->pass[i];
                    fx->pass[i] = fx->pass[active_count];
                    fx->pass[active_count] = temp;
                }
                ++active_count;
            }
        }
    }

    int changed = 0;
    for( int i = 0; i < array_count(fx->pass); ++i ) {
        char *name = postfx_name(fx, i); if( !name ) break;
        bool b = postfx_enabled(fx, i);
        if( ui_bool(name, &b) ) postfx_enable(fx, i, postfx_enabled(fx, i) ^ 1);
        ui_postfx(fx, i);
        ui_separator();
    }
    return changed;
}
static
int postfx_active_passes(postfx *fx) {
    int num_passes = 0;
    for (int i = 0; i < array_count(fx->pass); i++)
        if (fx->pass[i].enabled)
            ++num_passes;
    return num_passes;
}

static bool postfx_backbuffer_draw = false;

bool postfx_begin(postfx *fx, int width, int height) {
    // reset clear color: needed in case transparent window is being used (alpha != 0)
    glClearColor(0,0,0,0); // @transparent

    width += !width;
    height += !height;

    #if GLOBAL_FX_PASS_ENABLED
        if (array_count(fbos) <= 1) {
            postfx_backbuffer_draw = true;
            return false;
        }
    #endif

    // resize if needed
    if( fx->diffuse[0].w != width || fx->diffuse[0].h != height ) {
        texture_destroy(&fx->diffuse[0]);
        texture_destroy(&fx->diffuse[1]);
        texture_destroy(&fx->depth[0]);
        texture_destroy(&fx->depth[1]);
        fbo_destroy_id(fx->fb[0]);
        fbo_destroy_id(fx->fb[1]);

        // create texture, set texture parameters and content
        fx->diffuse[0] = texture_create(width, height, 4, NULL, TEXTURE_RGBA|TEXTURE_FLOAT);
        fx->depth[0] = texture_create(width, height, 1,  NULL, TEXTURE_DEPTH|TEXTURE_FLOAT);
        fx->fb[0] = fbo_id(fx->diffuse[0].id, fx->depth[0].id, 0);

        // create texture, set texture parameters and content
        fx->diffuse[1] = texture_create(width, height, 4, NULL, TEXTURE_RGBA|TEXTURE_FLOAT);
        fx->depth[1] = texture_create(width, height, 1, NULL, TEXTURE_DEPTH|TEXTURE_FLOAT);
        fx->fb[1] = fbo_id(fx->diffuse[1].id, fx->depth[1].id, 0);
    }

    uint64_t num_active_passes = postfx_active_passes(fx);
    bool active = fx->enabled && num_active_passes;
    if( !active ) {
        return false;
    }

    fbo_bind(fx->fb[1]);
    viewport_clear(true, true);
    viewport_clip(vec2(0,0), vec2(width, height));
    fbo_unbind();

    fbo_bind(fx->fb[0]);
    viewport_clear(true, true);
    viewport_clip(vec2(0,0), vec2(width, height));
    // we keep fbo_0 bound so that user can render into it.

    return true;
}

static renderstate_t postfx_rs;

void postfx_drawpass(postfx *fx, int pass, texture_t color, texture_t depth) {
    do_once {
        postfx_rs = renderstate();
        // disable depth test in 2d rendering
        postfx_rs.depth_test_enabled = 0;
        postfx_rs.cull_face_enabled = 0;
        postfx_rs.blend_enabled = 1;
        postfx_rs.blend_src = GL_SRC_ALPHA;
        postfx_rs.blend_dst = GL_ONE_MINUS_SRC_ALPHA;
    }
    postfx_drawpass_rs(fx, pass, color, depth, &postfx_rs);
}

void postfx_drawpass_rs(postfx *fx, int pass, texture_t color, texture_t depth, renderstate_t *rs) {
    passfx *p = postfx_pass(fx, pass);
    ASSERT(p);

    renderstate_apply(rs);

    int saved_vp[4];
    glGetIntegerv(GL_VIEWPORT, saved_vp);

    int width = saved_vp[2];
    int height = saved_vp[3];

    glUseProgram(p->program);

    int old_input_state = input_blocked();
    input_block(false);

    int frame = 0;
    float t = time_ms() / 1000.f;
    float w = width;
    float h = height;

    glBindVertexArray(fx->vao);

    if( 1 /* p->enabled */ ) {
        // bind texture to texture unit 0
glActiveTexture(GL_TEXTURE0 + 0); glBindTexture(GL_TEXTURE_2D, color.id);
        glUniform1i(p->uniforms[u_color], 0);

        glUniform1f(p->uniforms[u_channelres0x], color.w);
        glUniform1f(p->uniforms[u_channelres0y], color.h);
        
        // bind depth to texture unit 1
glActiveTexture(GL_TEXTURE0 + 1); glBindTexture(GL_TEXTURE_2D, depth.id);
        glUniform1i(p->uniforms[u_depth], 1);

        // bind uniforms
        static unsigned f = 0; ++f;
        glUniform1f(p->uniforms[u_time], t);
        glUniform1f(p->uniforms[u_frame], f-1);
        glUniform1f(p->uniforms[u_width], w);
        glUniform1f(p->uniforms[u_height], h);

        static vec4 smouse;
        if(input_down(MOUSE_L) || input_down(MOUSE_R) ) smouse.z = input(MOUSE_X), smouse.w = (window_height() - input(MOUSE_Y));
        if(input(MOUSE_L) || input(MOUSE_R)) smouse.x = input(MOUSE_X), smouse.y = (window_height() - input(MOUSE_Y));
        vec4 m = mul4(smouse, vec4(1,1,1-2*(!input(MOUSE_L) && !input(MOUSE_R)),1-2*(input_down(MOUSE_L) || input_down(MOUSE_R))));

        glUniform1f(p->uniforms[u_mousex], m.x);
        glUniform1f(p->uniforms[u_mousey], m.y);
        glUniform1f(p->uniforms[u_mousez], m.z);
        glUniform1f(p->uniforms[u_mousew], m.w);

        // fullscreen quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        profile_incstat("Render.num_drawcalls", +1);
        profile_incstat("Render.num_triangles", +2);
    }
    glBindVertexArray(0);
    glUseProgram(0);

    input_block(old_input_state);
}

bool postfx_end(postfx *fx, unsigned texture_id, unsigned depth_id) {
    uint64_t num_active_passes = postfx_active_passes(fx);
    bool active = fx->enabled && num_active_passes;
    if( !active ) {
        return false;
    }

    do_once {
        postfx_rs = renderstate();
        // disable depth test in 2d rendering
        postfx_rs.depth_test_enabled = 0;
        postfx_rs.cull_face_enabled = 0;
        postfx_rs.blend_enabled = 1;
        postfx_rs.blend_src = GL_ONE;
        postfx_rs.blend_dst = GL_ONE_MINUS_SRC_ALPHA;
    }

    // unbind postfx fbo
    fbo_unbind();

    renderstate_apply(&postfx_rs);

    int old_input_state = input_blocked();
    input_block(false);

    int frame = 0;
    float t = time_ms() / 1000.f;
    float w = fx->diffuse[0].w;
    float h = fx->diffuse[0].h;

    glBindVertexArray(fx->vao);

    if (texture_id == 0) {
        texture_id = fx->diffuse[0].id;
    }
    if (depth_id == 0) {
        depth_id = fx->depth[0].id;
    }


    unsigned first_pass = 1;

    for(int i = 0, e = array_count(fx->pass); i < e; ++i) {
        passfx *pass = &fx->pass[i];
        if( pass->enabled ) {
            if( !pass->program ) { --num_active_passes; continue; }
            glUseProgram(pass->program);

            // bind texture to texture unit 0
 glActiveTexture(GL_TEXTURE0 + 0); if(first_pass == 0) glBindTexture(GL_TEXTURE_2D, fx->diffuse[frame].id); else glBindTexture(GL_TEXTURE_2D, texture_id);
            glUniform1i(pass->uniforms[u_color], 0);

            glUniform1f(pass->uniforms[u_channelres0x], fx->diffuse[frame].w);
            glUniform1f(pass->uniforms[u_channelres0y], fx->diffuse[frame].h);
            
            // bind depth to texture unit 1
 glActiveTexture(GL_TEXTURE0 + 1); if(first_pass == 0) glBindTexture(GL_TEXTURE_2D, fx->depth[frame].id); else glBindTexture(GL_TEXTURE_2D, depth_id);
            glUniform1i(pass->uniforms[u_depth], 1);

            glUniform1f(pass->uniforms[u_channelres1x], fx->depth[frame].w);
            glUniform1f(pass->uniforms[u_channelres1y], fx->depth[frame].h);

            first_pass = 0;

            // bind uniforms
            static unsigned f = 0; ++f;
            glUniform1f(pass->uniforms[u_time], t);
            glUniform1f(pass->uniforms[u_frame], f-1);
            glUniform1f(pass->uniforms[u_width], w);
            glUniform1f(pass->uniforms[u_height], h);

            static vec4 smouse;
            if(input_down(MOUSE_L) || input_down(MOUSE_R) ) smouse.z = input(MOUSE_X), smouse.w = (window_height() - input(MOUSE_Y));
            if(input(MOUSE_L) || input(MOUSE_R)) smouse.x = input(MOUSE_X), smouse.y = (window_height() - input(MOUSE_Y));
            vec4 m = mul4(smouse, vec4(1,1,1-2*(!input(MOUSE_L) && !input(MOUSE_R)),1-2*(input_down(MOUSE_L) || input_down(MOUSE_R))));

            glUniform1f(pass->uniforms[u_mousex], m.x);
            glUniform1f(pass->uniforms[u_mousey], m.y);
            glUniform1f(pass->uniforms[u_mousez], m.z);
            glUniform1f(pass->uniforms[u_mousew], m.w);

            int bound = --num_active_passes;
            if (bound) fbo_bind(fx->fb[frame ^= 1]);

                // fullscreen quad
                glDrawArrays(GL_TRIANGLES, 0, 6);
                profile_incstat("Render.num_drawcalls", +1);
                profile_incstat("Render.num_triangles", +2);

            if (bound) fbo_unbind();
        }
    }
    glBindVertexArray(0);
    glUseProgram(0);

    // restore clear color: needed in case transparent window is being used (alpha != 0)
    glClearColor(0,0,0,1); // @transparent

    input_block(old_input_state);

    return true;
}

bool postfx_apply(postfx *fx, texture_t color_texture, texture_t depth_texture) {
    int saved_vp[4];
    glGetIntegerv(GL_VIEWPORT, saved_vp);
    if (!postfx_begin(fx, saved_vp[2], saved_vp[3])) {
        blit_rgb_flipped(color_texture);
        return false;
    }
    return postfx_end(fx, color_texture.id, depth_texture.id);
}

static postfx fx;
int fx_load_from_mem(const char *nameid, const char *content) {
    do_once if (!fx.vao) postfx_create(&fx, 0);
    return postfx_load_from_mem(&fx, nameid, content);
}
int fx_load(const char *filemask) {
    do_once if (!fx.vao) postfx_create(&fx, 0);
    return postfx_load(&fx, filemask);
}
bool fx_begin() {
    return postfx_begin(&fx, window_width(), window_height());
}
bool fx_begin_res(int w, int h) {
    return postfx_begin(&fx, w, h);
}
bool fx_end(unsigned texture_id, unsigned depth_id) {
    return postfx_end(&fx, texture_id, depth_id);
}
void fx_apply(texture_t color_texture, texture_t depth_texture) {
    postfx_apply(&fx, color_texture, depth_texture);
}
int fx_enabled(int pass) {
    return postfx_enabled(&fx, pass);
}
void fx_enable(int pass, int enabled) {
    postfx_enable(&fx, pass, enabled);
}
int fx_enable_ordered(int pass) {
    return postfx_enable_ordered(&fx, pass);
}
void fx_enable_all(int enabled) {
    postfx_enable_all(&fx, enabled);
}
char *fx_name(int pass) {
    return postfx_name(&fx, pass);
}
int fx_find(const char *name) {
    return postfx_find(&fx, name);
}
int fx_order(int pass, unsigned priority) {
    return postfx_order(&fx, pass, priority);
}
unsigned fx_program(int pass) {
    return postfx_program(&fx, pass);
}
void fx_setparam(int pass, const char *name, float value) {
    postfx_setparam(&fx, pass, name, value);
}
void fx_setparam3(int pass, const char *name, vec3 value) {
    postfx_setparam3(&fx, pass, name, value);
}
void fx_setparam4(int pass, const char *name, vec4 value) {
    postfx_setparam4(&fx, pass, name, value);
}
void fx_setparami(int pass, const char *name, int value) {
    postfx_setparami(&fx, pass, name, value);
}
void fx_setparamt(int pass, const char *name, texture_t value, int unit) {
    postfx_setparamt(&fx, pass, name, value, unit);
}
int ui_fx(int pass) {
    return ui_postfx(&fx, pass);
}
int ui_fxs() {
    return ui_postfxs(&fx);
}

void fx_drawpass_rs(int pass, texture_t color, texture_t depth, renderstate_t *rs) {
    postfx_drawpass_rs(&fx, pass, color, depth, rs);
}
void fx_drawpass(int pass, texture_t color, texture_t depth) {
    postfx_drawpass(&fx, pass, color, depth);
}


// multi-pass fx techniques

texture_t fxt_bloom(texture_t color, bloom_params_t params) {
    static postfx bloom = {0};
    static texture_t dummy = {0};
    static fbo_t result_fbo = {0};
    static int fbo_bloom = -1;
    static int fx_bloom_up = -1, fx_bloom_down = -1, fx_bloom_mix = -1, fx_gamma = -1;
    static int w = -1, h = -1;
    static array(texture_t) mips = 0;
    static renderstate_t bloom_rs = {0};

    int mips_count = 1;
    while ((color.w >> mips_count) > 1 && (color.h >> mips_count) > 1) {
        ++mips_count;
    }
    if (params.mips_count > 0) mips_count = clampi(mips_count, 1, params.mips_count);

    do_once {
        result_fbo = fbo(color.w, color.h, FBO_NO_DEPTH, TEXTURE_FLOAT);
        fbo_bloom = fbo_id(0,0,0);
        postfx_create(&bloom, 0);
        postfx_load(&bloom, "fxt/bloom/fxBloom*.fst");
        postfx_load(&bloom, "engine/art/fx/fxGamma.fs");
        fx_bloom_up = postfx_find(&bloom, "fxBloomUp.fst");
        fx_bloom_down = postfx_find(&bloom, "fxBloomDown.fst");
        fx_bloom_mix = postfx_find(&bloom, "fxBloomMix.fst");
        fx_gamma = postfx_find(&bloom, "fxGamma.fs");

        bloom_rs = renderstate();
        bloom_rs.depth_test_enabled = 0;
        bloom_rs.cull_face_enabled = 0;
        bloom_rs.blend_enabled = 1;
        bloom_rs.blend_src = GL_ONE;
        bloom_rs.blend_dst = GL_ONE;
        bloom_rs.blend_func = GL_FUNC_ADD;
    }

    if (w != color.w || h != color.h || array_count(mips) != mips_count) {
        w = color.w;
        h = color.h;

        for (int i = 0; i < array_count(mips); ++i) {
            texture_destroy(&mips[i]);
        }
        array_free(mips);
        array_resize(mips, mips_count);

        for (int i = 0; i < mips_count; ++i) {
            mips[i] = texture_create(w >> (i+1), h >> (i+1), 4, NULL, TEXTURE_FLOAT|TEXTURE_CLAMP|TEXTURE_LINEAR);
        }
    }

    int saved_vp[4];
    glGetIntegerv(GL_VIEWPORT, saved_vp);

    fbo_resize(&result_fbo, color.w, color.h);

    fbo_bind(result_fbo.id);
    viewport_clear(true, true);
    glViewport(0, 0, color.w, color.h);
    postfx_setparam(&bloom, fx_gamma, "u_gamma", 1.0/2.2);
    postfx_drawpass(&bloom, fx_gamma, color, dummy);
    fbo_unbind();

    texture_t *current_mip = &result_fbo.texture_color;

    for (int i = 0; i < mips_count; ++i) {
        fbo_bind(fbo_bloom);
        fbo_attach(fbo_bloom, 0, mips[i]);
        viewport_clear(true, true);
        glViewport(0, 0, mips[i].w, mips[i].h);
        postfx_setparami(&bloom, fx_bloom_down, "miplevel", i);
        postfx_setparam(&bloom, fx_bloom_down, "threshold", params.threshold);
        postfx_setparam(&bloom, fx_bloom_down, "soft_threshold", params.soft_threshold);
        postfx_setparami(&bloom, fx_bloom_down, "karis_disabled", !params.suppress_fireflies);
        postfx_drawpass(&bloom, fx_bloom_down, *current_mip, dummy);
        fbo_unbind();
        current_mip = &mips[i];
    }

    for (int i = mips_count-1; i > 0; --i) {
        texture_t src = mips[i];
        texture_t dst = mips[i-1];

        fbo_bind(fbo_bloom);
        fbo_attach(fbo_bloom, 0, dst);
        glViewport(0, 0, dst.w, dst.h);
        postfx_setparam(&bloom, fx_bloom_up, "filterRadius", params.filter_radius);
        postfx_drawpass_rs(&bloom, fx_bloom_up, src, dummy, &bloom_rs);
        fbo_unbind();
    }

    fbo_bind(result_fbo.id);
    viewport_clear(true, true);
    glViewport(0, 0, color.w, color.h);
    postfx_setparam(&bloom, fx_bloom_mix, "filterRadius", params.filter_radius);
    postfx_setparam(&bloom, fx_bloom_mix, "strength", params.strength);
    postfx_drawpass(&bloom, fx_bloom_mix, mips[0], dummy);
    fbo_unbind();

    glViewport(saved_vp[0], saved_vp[1], saved_vp[2], saved_vp[3]);
    return result_fbo.texture_color;
}

texture_t fxt_reflect(texture_t color, texture_t depth, texture_t normal, texture_t matprops, mat44 proj, mat44 view, reflect_params_t params) {
    static postfx reflect = {0};
    static fbo_t downsample_fbo = {0};
    static int fx_reflect = -1;

    int saved_vp[4];
    glGetIntegerv(GL_VIEWPORT, saved_vp);

    while ((color.w >> params.downsample) < 1 || (color.h >> params.downsample) < 1) {
        params.downsample--;
    }
    params.downsample = max(params.downsample, 0);

    unsigned w = color.w >> params.downsample;
    unsigned h = color.h >> params.downsample;

    do_once {
        downsample_fbo = fbo(w, h, FBO_NO_DEPTH, TEXTURE_FLOAT|TEXTURE_LINEAR|TEXTURE_RGBA);
        postfx_create(&reflect, 0);
        postfx_load(&reflect, "fxt/reflect/fxReflect.fst");
        fx_reflect = postfx_find(&reflect, "fxReflect.fst");
    }

    fbo_resize(&downsample_fbo, w, h);

    unsigned old_shader = last_shader;
    shader_bind(postfx_program(&reflect, fx_reflect));

    mat44 inv_proj, inv_view;
    invert44(inv_proj, proj);
    invert44(inv_view, view);

    shader_mat44("u_projection", proj);
    shader_mat44("u_inv_projection", inv_proj);
    shader_mat44("u_view", view);
    shader_mat44("u_inv_view", inv_view);
    shader_texture("u_normal_texture", normal.id, 2);
    shader_texture("u_matprops_texture", matprops.id, 3);
    shader_texture("u_cubemap_texture", params.cubemap ? params.cubemap->id : 0, 4);
    shader_float("u_metallic_threshold", params.metallic_threshold);
    shader_float("u_max_distance", params.max_distance);
    shader_float("u_reflection_strength", params.reflection_strength);
    shader_int("u_sample_skybox", params.cubemap ? 1 : 0);
    shader_bool("u_disabled", params.disabled);
    fbo_bind(downsample_fbo.id);
    viewport_clear(true, true);
    viewport_area(vec2(0,0), vec2(w, h));
    postfx_drawpass(&reflect, fx_reflect, color, depth);
    fbo_unbind();

    shader_bind(old_shader);

    glViewport(saved_vp[0], saved_vp[1], saved_vp[2], saved_vp[3]);
    return downsample_fbo.texture_color;
}

#endif
