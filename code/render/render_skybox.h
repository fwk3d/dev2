// -----------------------------------------------------------------------------
// skyboxes

#if !CODE

enum SKYBOX_FLAGS {
	SKYBOX_RAYLEIGH,
	SKYBOX_CUBEMAP,
	SKYBOX_PBR,
};

typedef struct skybox_t {
    handle program, rayleigh_program;
    mesh_t geometry;
    cubemap_t cubemap;
    int flags;
    bool rayleigh_immediate;

    // pbr
    texture_t sky, refl, env;
} skybox_t;

API skybox_t skybox(const char *panorama, int flags);
API skybox_t skybox_pbr(const char *sky_map, const char *refl_map, const char *env_map);
API int      skybox_render(skybox_t *sky, mat44 proj, mat44 view);
API void     skybox_destroy(skybox_t *sky);
API void     skybox_calc_sh(skybox_t *probe, skybox_t *sky, float sky_intensity);
API void     skybox_mie_calc_sh(skybox_t *sky, float sky_intensity);
API void     skybox_sh_reset(skybox_t *sky);  /* @deprecated */
API void     skybox_sh_shader(skybox_t *sky);  /* @deprecated */
API void     skybox_sh_add_light(skybox_t *sky, vec3 light, vec3 dir, float strength);  /* @deprecated */

API int      skybox_push_state(skybox_t *sky, mat44 proj, mat44 view); // @to deprecate
API int      skybox_pop_state(); // @to deprecate

#else

// -----------------------------------------------------------------------------
// skyboxes

static inline
texture_t load_env_tex( const char *pathfile, unsigned flags ) {
    stbi_hdr_to_ldr_gamma(2.2f);
    int flags_hdr = strendi(pathfile, ".hdr") ? TEXTURE_FLOAT | TEXTURE_RGBA : 0;
    texture_t t = texture_compressed(pathfile, flags | TEXTURE_LINEAR | TEXTURE_REPEAT | TEXTURE_UNIQUE | flags_hdr);
    glBindTexture( GL_TEXTURE_2D, t.id );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    return t;
}

skybox_t skybox(const char *asset, int flags) {
    skybox_t sky = {0};

    // sky mesh
    vec3 vertices[] = {{+1,-1,+1},{+1,+1,+1},{+1,+1,-1},{-1,+1,-1},{+1,-1,-1},{-1,-1,-1},{-1,-1,+1},{-1,+1,+1}};
    unsigned indices[] = { 0, 1, 2, 3, 4, 5, 6, 3, 7, 1, 6, 0, 4, 2 };
    mesh_update(&sky.geometry, "p3", 0,countof(vertices),vertices, countof(indices),indices, MESH_TRIANGLE_STRIP);

    // sky program
    sky.flags = flags && flags != SKYBOX_PBR ? flags : !!asset ? SKYBOX_CUBEMAP : SKYBOX_RAYLEIGH; // either cubemap or rayleigh
    sky.program = shader(file_read("shaders/skybox_vs.gl",0),
        file_read("shaders/skybox_fs.gl",0),
        "att_position", "fragcolor", NULL);
    sky.rayleigh_program = shader(file_read("shaders/skybox_vs.gl",0),
        file_read("shaders/skybox_rayleigh_fs.gl",0),
        "att_position", "fragcolor", NULL);

    // sky cubemap & SH
    if( asset ) {
        int is_panorama = file_size( asset );
        if( is_panorama ) { // is file
            stbi_hdr_to_ldr_gamma(2.2f);
            texture_t panorama = load_env_tex( asset, 0 );
            sky.cubemap = cubemap( panorama, 0 );
            skybox_t probe = {0};
            skybox_calc_sh(&probe, &sky, 1.0/2.2);
            memcpy(sky.cubemap.sh, probe.cubemap.sh, 9 * sizeof(vec3));
            skybox_destroy(&probe);
        } else { // is folder
            PRINTF("[warn] Folder-based skyboxes are not supported anymore!");
        }
    } else {
        // set up mie defaults // @fixme: use shader params instead
        sky.rayleigh_immediate = true;
        shader_bind(sky.rayleigh_program);
        shader_vec3("uSunPos", vec3( 0, 0.1, -1 ));
        shader_vec3("uRayOrigin", vec3(0.0, 6372000.0, 0.0));
        shader_float("uSunIntensity", 22.0);
        shader_float("uPlanetRadius", 6371000.0);
        shader_float("uAtmosphereRadius", 6471000.0);
        shader_vec3("uRayleighScattering", vec3(5.5e-6, 13.0e-6, 22.4e-6));
        shader_float("uMieScattering", 21e-6);
        shader_float("uRayleighScaleHeight", 8000.0);
        shader_float("uMieScaleHeight", 1200.0);
        shader_float("uMiePreferredDirection", 0.758);
        skybox_mie_calc_sh(&sky, 1.2);
    }

    return sky;
}

skybox_t skybox_pbr(const char *sky_map, const char *refl_map, const char *env_map) {
    skybox_t sky = {0};

    // sky mesh
    vec3 vertices[] = {{+1,-1,+1},{+1,+1,+1},{+1,+1,-1},{-1,+1,-1},{+1,-1,-1},{-1,-1,-1},{-1,-1,+1},{-1,+1,+1}};
    unsigned indices[] = { 0, 1, 2, 3, 4, 5, 6, 3, 7, 1, 6, 0, 4, 2 };
    mesh_update(&sky.geometry, "p3", 0,countof(vertices),vertices, countof(indices),indices, MESH_TRIANGLE_STRIP);

    // sky program
    sky.flags = SKYBOX_PBR;
    sky.program = shader(file_read("shaders/skybox_vs.gl",0),
        file_read("skybox_fs.gl",0),
        "att_position", "fragcolor", NULL);

    // sky cubemap & SH
    if( sky_map ) {
        int is_panorama = file_size( sky_map );
        if( is_panorama ) { // is file
            stbi_hdr_to_ldr_gamma(2.2f);
            texture_t panorama = load_env_tex( sky_map, 0 );
            sky.cubemap = cubemap( panorama, 0 );
            skybox_t probe = {0};
            skybox_calc_sh(&probe, &sky, 1.0/2.2);
            memcpy(sky.cubemap.sh, probe.cubemap.sh, 9 * sizeof(vec3));
            skybox_destroy(&probe);
        } else { // is folder
            PRINTF("[warn] Folder-based skyboxes are not supported anymore!");
        }
    }
    if( refl_map ) {
        sky.refl = load_env_tex(refl_map, TEXTURE_MIPMAPS);
    }
    if( env_map ) {
        sky.env = load_env_tex(env_map, TEXTURE_MIPMAPS);
    }

    return sky;
}

static renderstate_t skybox_rs;
API vec4 window_getcolor_(); // internal use, not public

static inline
void skybox_render_rayleigh(skybox_t *sky, mat44 proj, mat44 view) {
    last_cubemap = &sky->cubemap;

    do_once {
        skybox_rs = renderstate();
        skybox_rs.depth_test_enabled = 1;
        skybox_rs.cull_face_enabled = 0;
        skybox_rs.front_face = GL_CCW;
    }

    // we have to reset clear color here, because of wrong alpha compositing issues on native transparent windows otherwise
    // vec4 bgcolor = window_getcolor_(); 
    // skybox_rs.clear_color[0] = bgcolor.r;
    // skybox_rs.clear_color[1] = bgcolor.g;
    // skybox_rs.clear_color[2] = bgcolor.b;
    // skybox_rs.clear_color[3] = 1; // @transparent

    mat44 mvp; multiply44x2(mvp, proj, view);

    //glDepthMask(GL_FALSE);
    shader_bind(sky->rayleigh_program);
    shader_mat44("u_mvp", mvp);

    renderstate_apply(&skybox_rs);
    mesh_render(&sky->geometry);
}

void skybox_calc_sh(skybox_t *probe, skybox_t *sky, float sky_intensity) {
    cubemap_beginbake(&probe->cubemap, vec3(0, 0, 0), 1024, 1024);
    mat44 proj, view;
    while (cubemap_stepbake(&probe->cubemap, proj, view)) {
        skybox_render(sky, proj, view);
    }
    cubemap_endbake(&probe->cubemap, 0, sky_intensity);
}

void skybox_mie_calc_sh(skybox_t *sky, float sky_intensity) {
    cubemap_beginbake(&sky->cubemap, vec3(0, 0, 0), 1024, 1024);
    mat44 proj, view;
    while (cubemap_stepbake(&sky->cubemap, proj, view)) {
        skybox_render_rayleigh(sky, proj, view);
    }
    cubemap_endbake(&sky->cubemap, 0, sky_intensity);
}

void skybox_sh_add_light(skybox_t *sky, vec3 light, vec3 dir, float strength) {
    cubemap_sh_addlight(&sky->cubemap, light, dir, strength);
}

int skybox_push_state(skybox_t *sky, mat44 proj, mat44 view) {
    last_cubemap = &sky->cubemap;

    do_once {
        skybox_rs = renderstate();
        skybox_rs.depth_test_enabled = 1;
        skybox_rs.cull_face_enabled = 0;
        skybox_rs.front_face = GL_CCW;
        skybox_rs.depth_func = GL_LEQUAL;
        skybox_rs.reverse_z = 0;
    }

    // we have to reset clear color here, because of wrong alpha compositing issues on native transparent windows otherwise
    // vec4 bgcolor = window_getcolor_(); 
    // skybox_rs.clear_color[0] = bgcolor.r;
    // skybox_rs.clear_color[1] = bgcolor.g;
    // skybox_rs.clear_color[2] = bgcolor.b;
    // skybox_rs.clear_color[3] = 1; // @transparent



    mat44 mvp;
    multiply44x2(mvp, proj, view);

    //glDepthMask(GL_FALSE);
    shader_bind(sky->program);
    shader_mat44("u_mvp", mvp);
    shader_texture("u_skybox", sky->cubemap.id, 0);

    renderstate_apply(&skybox_rs);
    return 0; // @fixme: return sortable hash here?
}
int skybox_pop_state() {
    //vec4 bgcolor = window_getcolor_(); glClearColor(bgcolor.r, bgcolor.g, bgcolor.b, window_has_transparent() ? 0 : bgcolor.a); // @transparent
    // glClearDepth(skybox_rs.reverse_z ? 0.0f : 1.0f);
    // glClear(GL_DEPTH_BUFFER_BIT);
    return 0;
}
int skybox_render(skybox_t *sky, mat44 proj, mat44 view) {
    if (sky->rayleigh_immediate && !sky->flags) {
        skybox_render_rayleigh(sky, proj, view);
        return 0;
    }
    skybox_push_state(sky, proj, view);
    mesh_render(&sky->geometry);
    skybox_pop_state();
    return 0;
}
void skybox_destroy(skybox_t *sky) {
    glDeleteProgram(sky->program);
    glDeleteProgram(sky->rayleigh_program);
    cubemap_destroy(&sky->cubemap);
    mesh_destroy(&sky->geometry);
}

#endif
