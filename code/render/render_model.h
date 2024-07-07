// -----------------------------------------------------------------------------
// models

#if !CODE

enum MODEL_FLAGS {
    MODEL_NO_ANIMATIONS = 1,
    MODEL_NO_MESHES = 2,
    MODEL_NO_TEXTURES = 4,
    MODEL_NO_FILTERING = 8,
    MODEL_STREAM = 16, // useful with model_sync()

    // model cache
    MODEL_CACHED = 0, // shared mesh, unique materials
    MODEL_SHARED = 512, // shared mesh, shared materials
    MODEL_UNIQUE = 1024, // unique mesh, unique materials

    // internal
    MODEL_PROCEDURAL = 2048,
};

enum FOG_MODE {
    FOG_NONE,
    FOG_LINEAR,
    FOG_EXP,
    FOG_EXP2,
    FOG_DEPTH,
};

enum RENDER_PASS {
    RENDER_PASS_OPAQUE,
    RENDER_PASS_TRANSPARENT,

    RENDER_PASS_OVERRIDES_BEGIN,
    
    RENDER_PASS_MATERIAL,
    RENDER_PASS_SHADOW,
    
    RENDER_PASS_CUSTOM, // make sure to apply renderstate before calling this
    RENDER_PASS_OVERRIDES_END,
    
    NUM_RENDER_PASSES
};

enum MODEL_TEXTURE_SLOTS {
    // PBR
    MODEL_TEXTURE_ALBEDO,
    MODEL_TEXTURE_NORMALS,
    MODEL_TEXTURE_ROUGHNESS,
    MODEL_TEXTURE_METALLIC,
    MODEL_TEXTURE_AO,
    MODEL_TEXTURE_AMBIENT,
    MODEL_TEXTURE_EMISSIVE,
    MODEL_TEXTURE_PARALLAX,
    
    // IBL
    MODEL_TEXTURE_ENV_CUBEMAP,
    MODEL_TEXTURE_SKYSPHERE,
    MODEL_TEXTURE_SKYENV,
    MODEL_TEXTURE_BRDF_LUT,

    // Shadows
    MODEL_TEXTURE_SHADOW_OFFSETS,

    MODEL_TEXTURE_SHADOW_MAP_2D,
    MODEL_TEXTURE_SHADOW_MAP_2D_COUNT = MODEL_TEXTURE_SHADOW_MAP_2D+NUM_SHADOW_CASCADES,

    MODEL_TEXTURE_SHADOW_MAP_CUBEMAP,
    MODEL_TEXTURE_SHADOW_MAP_CUBEMAP_COUNT = MODEL_TEXTURE_SHADOW_MAP_CUBEMAP+MAX_SHADOW_LIGHTS,

    // User-defined slot
    MODEL_TEXTURE_USER_DEFINED,

    NUM_MODEL_TEXTURE_SLOTS
};

typedef struct lightarray_t {
    light_t *base;
    unsigned count;
} lightarray_t;

typedef struct model_shaderinfo_t {
    char *vs;
    char *fs;
    char *defines;
    array(char*) switches;
} model_shaderinfo_t;

typedef struct model_t {
    struct iqm_t *iqm; // private
    char *filename;

    unsigned num_textures;
    char **texture_names;
    array(material_t) materials;
    
    texture_t sky_refl, sky_env, sky_cubemap;

    unsigned num_meshes;
    unsigned num_triangles;
    unsigned num_joints; // num_poses;
    unsigned num_anims;
    unsigned num_frames;
    shadowmap_t *shadow_map;
    lightarray_t lights;
    bool shadow_receiver;
    float curframe;
    mat44 pivot;

    int stride; // usually 68 bytes for a p3 u2 u2 n3 t4 i4B w4B c4B vertex stream
    void *verts;
    int num_verts;
    void *tris;
    int num_tris;
    handle vao, ibo, vbo, vao_instanced;

    array(int) lod_collapse_map; // to which neighbor each vertex collapses. ie, [10] -> 7 (used by LODs) @leak
    void *lod_verts;
    int lod_num_verts;
    void *lod_tris;
    int lod_num_tris;

    unsigned flags;
    unsigned billboard;

    float *instanced_matrices;
    unsigned num_instances;

    int stored_flags;
    renderstate_t rs[NUM_RENDER_PASSES];

    bool frustum_enabled;
    frustum frustum_state;

    array(model_uniform_t) uniforms;
    model_shaderinfo_t shaderinfo;
} model_t;

typedef struct model_vertex_t {
    vec3 position;
    vec2 texcoord;
    vec3 normal;
    vec4 tangent;
    uint8_t blend_indices[4];
    uint8_t blend_weights[4];
    float blend_vertex_index;
    vec4 color;
    vec2 texcoord2;
} model_vertex_t;

enum BILLBOARD_MODE {
    BILLBOARD_X = 0x1,
    BILLBOARD_Y = 0x2,
    BILLBOARD_Z = 0x4,

    BILLBOARD_CYLINDRICAL = BILLBOARD_X|BILLBOARD_Z,
    BILLBOARD_SPHERICAL = BILLBOARD_X|BILLBOARD_Y|BILLBOARD_Z
};

API model_t  model(const char *filename, int flags); //< filename == 0 for procedural models
API model_t  model_from_mem(const void *mem, int sz, int flags); //< mem == 0 for procedural models
API void     model_sync(model_t m, int num_vertices, model_vertex_t *vertices, int num_indices, uint32_t *indices); //< MODEL_PROCEDURAL models only
API float    model_animate(model_t, float curframe);
API float    model_animate_clip(model_t, float curframe, int minframe, int maxframe, bool loop);
API float    model_animate_blends(model_t m, anim_t *primary, anim_t *secondary, float delta);
API aabb     model_aabb(model_t, mat44 transform);
API sphere   model_bsphere(model_t, mat44 transform);
API void     model_setshader(model_t*, const char *vs, const char *fs, const char *defines);
API void     model_adduniform(model_t*, model_uniform_t uniform);
API void     model_adduniforms(model_t*, unsigned count, model_uniform_t *uniforms);
API void     model_addswitch(model_t*, const char *name);
API void     model_delswitch(model_t*, const char *name);
API uint32_t model_uniforms_checksum(unsigned count, model_uniform_t *uniforms);
API void     model_fog(model_t*, unsigned mode, vec3 color, float start, float end, float density);
API void     model_skybox(model_t*, skybox_t sky);
API void     model_cubemap(model_t*, cubemap_t *c);
API void     model_probe(model_t*, vec3 center, float radius, unsigned count, cubemap_t *c);
API void     model_shadow(model_t*, shadowmap_t *sm);
API void     model_light(model_t*, unsigned count, light_t *lights);
API void     model_render(model_t *mdl, mat44 proj, mat44 view, mat44* models, unsigned count, int pass); // pass == -1 to render all common passes
API void     model_skeletonrender(model_t, mat44 model);
API bool     model_has_transparency_mesh(model_t m, int mesh);
API bool     model_has_transparency(model_t m);
API void     model_frustumset(model_t *m, frustum f);
API void     model_frustumclear(model_t *m);
API bool     model_bonegetpose(model_t m, unsigned joint, mat34 *out);
API bool     model_bonegetposition(model_t m, unsigned joint, mat44 M, vec3 *out);
API void     model_destroy(model_t);

API vec3     pose(bool forward, float curframe, int minframe, int maxframe, bool loop, float *opt_retframe);

API void ui_materials(model_t *m);

API void     drawmat_render(drawmat_t *lookup, model_t m, mat44 proj, mat44 view, mat44* models, unsigned count);

#define each_material(m, material) each_array_ptr(m.materials, material_t, material)

#else

// -----------------------------------------------------------------------------
// skeletal meshes (iqm)

#define IQM_MAGIC "INTERQUAKEMODEL"
#define IQM_VERSION 2

struct iqmheader {
    char magic[16];
    unsigned version;
    unsigned filesize;
    unsigned flags;
    unsigned num_text, ofs_text;
    unsigned num_meshes, ofs_meshes;
    unsigned num_vertexarrays, num_vertexes, ofs_vertexarrays;
    unsigned num_triangles, ofs_triangles, ofs_adjacency;
    unsigned num_joints, ofs_joints;
    unsigned num_poses, ofs_poses;
    unsigned num_anims, ofs_anims;
    unsigned num_frames, num_framechannels, ofs_frames, ofs_bounds;
    unsigned num_comment, ofs_comment;
    unsigned num_extensions, ofs_extensions;
};

struct iqmmesh {
    unsigned name;
    unsigned material;
    unsigned first_vertex, num_vertexes;
    unsigned first_triangle, num_triangles;
};

enum {
    IQM_POSITION,
    IQM_TEXCOORD,
    IQM_NORMAL,
    IQM_TANGENT,
    IQM_BLENDINDEXES,
    IQM_BLENDWEIGHTS,
    IQM_COLOR,
    IQM_CUSTOM = 0x10
};

enum {
    IQM_BYTE,
    IQM_UBYTE,
    IQM_SHORT,
    IQM_USHORT,
    IQM_INT,
    IQM_UINT,
    IQM_HALF,
    IQM_FLOAT,
    IQM_DOUBLE,
};

struct iqmtriangle {
    unsigned vertex[3];
};

struct iqmadjacency {
    unsigned triangle[3];
};

struct iqmjoint {
    unsigned name;
    int parent;
    float translate[3], rotate[4], scale[3];
};

struct iqmpose {
    int parent;
    unsigned mask;
    float channeloffset[10];
    float channelscale[10];
};

struct iqmanim {
    unsigned name;
    unsigned first_frame, num_frames;
    float framerate;
    unsigned flags;
};

enum {
    IQM_LOOP = 1<<0
};

struct iqmvertexarray {
    unsigned type;
    unsigned flags;
    unsigned format;
    unsigned size;
    unsigned offset;
};

struct iqmbounds {
    union {
        struct { float bbmin[3], bbmax[3]; };
        struct { vec3 min3, max3; };
        aabb box;
    };
    float xyradius, radius;
};

// -----------------------------------------------------------------------------

typedef struct iqm_vertex {
    GLfloat position[3];
    GLfloat texcoord[2];
    GLfloat normal[3];
    GLfloat tangent[4];
    GLubyte blendindexes[4];
    GLubyte blendweights[4];
    GLfloat blendvertexindex;
    GLfloat color[4];
    GLfloat texcoord2[2];
} iqm_vertex;

STATIC_ASSERT((sizeof(iqm_vertex) == sizeof(model_vertex_t)));

typedef struct iqm_t {
    int nummeshes, numtris, numverts, numjoints, numframes, numanims;
    GLuint vao, ibo, vbo;
    uint8_t *buf, *meshdata, *animdata;
    unsigned *mesh_materials;
    struct iqmmesh *meshes;
    struct iqmjoint *joints;
    struct iqmpose *poses;
    struct iqmanim *anims;
    struct iqmbounds *bounds;
    mat34 *baseframe, *inversebaseframe, *outframe, *frames;
    GLint bonematsoffset;
    uint32_t instancing_checksum;
    int uniforms[3][NUM_MODEL_UNIFORMS];
    handle program;
    handle shadow_program;
    handle material_program;
    unsigned light_ubo;
    int texture_unit;
} iqm_t;

//
// model binds
//

bool model_compareuniform(const model_uniform_t *a, const model_uniform_t *b) {
    if (a->kind != b->kind) return false;
    if (strcmp(a->name, b->name) != 0) return false;

    switch (a->kind) {
        case UNIFORM_FLOAT:
            if (a->f != b->f) return false;
            break;
        case UNIFORM_INT:
        case UNIFORM_UINT:
        case UNIFORM_BOOL:
        case UNIFORM_SAMPLER2D:
        case UNIFORM_SAMPLER3D:
        case UNIFORM_SAMPLERCUBE:
            if (a->i != b->i) return false;
            break;
        case UNIFORM_VEC2:
            if (memcmp(&a->v2.x, &b->v2.x, sizeof(vec2)) != 0) return false;
            break;
        case UNIFORM_VEC3:
            if (memcmp(&a->v3.x, &b->v3.x, sizeof(vec3)) != 0) return false;
            break;
        case UNIFORM_VEC4:
            if (memcmp(&a->v4.x, &b->v4.x, sizeof(vec4)) != 0) return false;
            break;
        case UNIFORM_MAT3:
            if (memcmp(a->m33, b->m33, sizeof(mat33)) != 0) return false;
            break;
        case UNIFORM_MAT4:
            if (memcmp(a->m44, b->m44, sizeof(mat44)) != 0) return false;
            break;
        default:
            return false;
    }
    return true;
}

bool model_compareuniforms(unsigned s1, const model_uniform_t *a, unsigned s2, const model_uniform_t *b) {
    if (s1 != s2) return false;
    
    for (unsigned i = 0; i < s1; ++i) {
        if (!model_compareuniform(&a[i], &b[i])) return false;
    }
    
    return true;
}

uint32_t model_uniforms_checksum(unsigned count, model_uniform_t *uniforms) {
    uint32_t checksum = 0;
    for (unsigned i = 0; i < count; ++i) {
        checksum ^= hh_str(uniforms[i].name);
        checksum ^= uniforms[i].kind;
        
        switch (uniforms[i].kind) {
            case UNIFORM_FLOAT:
                checksum ^= hh_float(uniforms[i].f);
                break;
            case UNIFORM_INT:
            case UNIFORM_UINT:
            case UNIFORM_BOOL:
            case UNIFORM_SAMPLER2D:
            case UNIFORM_SAMPLER3D:
            case UNIFORM_SAMPLERCUBE:
                checksum ^= hh_int(uniforms[i].i);
                break;
            case UNIFORM_VEC2:
                checksum ^= hh_vec2(uniforms[i].v2);
                break;
            case UNIFORM_VEC3:
                checksum ^= hh_vec3(uniforms[i].v3);
                break;
            case UNIFORM_VEC4:
                checksum ^= hh_vec4(uniforms[i].v4);
                break;
            case UNIFORM_MAT3:
                checksum ^= hh_mat33(uniforms[i].m33);
                break;
            case UNIFORM_MAT4:
                checksum ^= hh_mat44(uniforms[i].m44);
                break;
        }
    }
    return checksum;
}

static inline
void model_init_uniforms(iqm_t *q, int slot, int program) {
    if(!q) return;

    for (int i = 0; i < NUM_MODEL_UNIFORMS; ++i) q->uniforms[slot][i] = -1;
    unsigned shader = program;
    int loc = -1;
    glUseProgram(shader);

    // MV Matrix
    if ((loc = glGetUniformLocation(shader, "u_mv")) >= 0 || (loc = glGetUniformLocation(shader, "MV")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_MV] = loc;

    // MVP Matrix
    if ((loc = glGetUniformLocation(shader, "u_mvp")) >= 0 || (loc = glGetUniformLocation(shader, "MVP")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_MVP] = loc;


    // VP Matrix
    if ((loc = glGetUniformLocation(shader, "u_vp")) >= 0 || (loc = glGetUniformLocation(shader, "VP")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_VP] = loc;

    // Camera Position
    if ((loc = glGetUniformLocation(shader, "u_cam_pos")) >= 0 || (loc = glGetUniformLocation(shader, "cam_pos")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_CAM_POS] = loc;

    // Camera Direction
    if ((loc = glGetUniformLocation(shader, "u_cam_dir")) >= 0 || (loc = glGetUniformLocation(shader, "cam_dir")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_CAM_DIR] = loc;

    // Billboard
    if ((loc = glGetUniformLocation(shader, "u_billboard")) >= 0 || (loc = glGetUniformLocation(shader, "billboard")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_BILLBOARD] = loc;

    // Model Matrix
    if ((loc = glGetUniformLocation(shader, "M")) >= 0 || (loc = glGetUniformLocation(shader, "model")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_MODEL] = loc;

    // View Matrix
    if ((loc = glGetUniformLocation(shader, "V")) >= 0 || (loc = glGetUniformLocation(shader, "view")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_VIEW] = loc;

    // Instanced
    if ((loc = glGetUniformLocation(shader, "u_instanced")) >= 0 || (loc = glGetUniformLocation(shader, "instanced")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_INSTANCED] = loc;

    // Inverse View Matrix
    if ((loc = glGetUniformLocation(shader, "inv_view")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_INV_VIEW] = loc;

    // Projection Matrix
    if ((loc = glGetUniformLocation(shader, "P")) >= 0 || (loc = glGetUniformLocation(shader, "proj")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_PROJ] = loc;

    // Skinned
    if ((loc = glGetUniformLocation(shader, "SKINNED")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_SKINNED] = loc;

    // Bone Matrix
    if ((loc = glGetUniformLocation(shader, "vsBoneMatrix")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_VS_BONE_MATRIX] = loc;

    // Matcaps
    if ((loc = glGetUniformLocation(shader, "u_matcaps")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_U_MATCAPS] = loc;

    // Frame Count
    if ((loc = glGetUniformLocation(shader, "frame_count")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_FRAME_COUNT] = loc;

    // Frame Time
    if ((loc = glGetUniformLocation(shader, "frame_time")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_FRAME_TIME] = loc;

    // Shadow Uniforms
    if ((loc = glGetUniformLocation(shader, "cameraToShadowView")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_SHADOW_CAMERA_TO_SHADOW_VIEW] = loc;

    if ((loc = glGetUniformLocation(shader, "cameraToShadowProjector")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_SHADOW_CAMERA_TO_SHADOW_PROJECTOR] = loc;

    if ((loc = glGetUniformLocation(shader, "shadow_technique")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_SHADOW_TECHNIQUE] = loc;

    if ((loc = glGetUniformLocation(shader, "u_shadow_receiver")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_U_SHADOW_RECEIVER] = loc;

    if ((loc = glGetUniformLocation(shader, "u_blend_region")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_U_BLEND_REGION] = loc;

    if ((loc = glGetUniformLocation(shader, "shadow_offsets")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_SHADOW_OFFSETS] = loc;

    if ((loc = glGetUniformLocation(shader, "shadow_filter_size")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_SHADOW_FILTER_SIZE] = loc;

    if ((loc = glGetUniformLocation(shader, "shadow_window_size")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_SHADOW_WINDOW_SIZE] = loc;

    // PBR Uniforms
    if ((loc = glGetUniformLocation(shader, "resolution")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_RESOLUTION] = loc;

    if ((loc = glGetUniformLocation(shader, "has_tex_skycube")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_HAS_TEX_ENV_CUBEMAP] = loc;

    if ((loc = glGetUniformLocation(shader, "tex_skycube")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_TEX_ENV_CUBEMAP] = loc;

    if ((loc = glGetUniformLocation(shader, "has_tex_skysphere")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_HAS_TEX_SKYSPHERE] = loc;

    if ((loc = glGetUniformLocation(shader, "has_tex_skyenv")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_HAS_TEX_SKYENV] = loc;

    if ((loc = glGetUniformLocation(shader, "tex_skysphere")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_TEX_SKYSPHERE] = loc;

    if ((loc = glGetUniformLocation(shader, "skysphere_mip_count")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_SKYSPHERE_MIP_COUNT] = loc;

    if ((loc = glGetUniformLocation(shader, "tex_skyenv")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_TEX_SKYENV] = loc;

    if ((loc = glGetUniformLocation(shader, "tex_brdf_lut")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_TEX_BRDF_LUT] = loc;

    if ((loc = glGetUniformLocation(shader, "u_max_shadow_cascades")) >= 0)
        q->uniforms[slot][MODEL_UNIFORM_MAX_SHADOW_CASCADES] = loc;

    for (int j = 0; j < NUM_SHADOW_CASCADES; j++) {
        if ((loc = glGetUniformLocation(shader, va("u_cascade_distances[%d]", j))) >= 0)
            q->uniforms[slot][MODEL_UNIFORM_SHADOW_CASCADE_DISTANCES + j] = loc;
        if ((loc = glGetUniformLocation(shader, va("shadowMap2D[%d]", j))) >= 0)
            q->uniforms[slot][MODEL_UNIFORM_SHADOW_MAP_2D + j] = loc;
    }
    for (int i = 0; i < MAX_LIGHTS; i++) {
        if ((loc = glGetUniformLocation(shader, va("shadowMap[%d]", i))) >= 0)
            q->uniforms[slot][MODEL_UNIFORM_SHADOW_MAP_CUBEMAP+i] = loc;
    }
}


static int model_totalTextureUnits = 0;

static inline
int model_texture_unit(model_t *m) {
    do_once glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &model_totalTextureUnits);
    // ASSERT(textureUnit < totalTextureUnits, "%d texture units exceeded", totalTextureUnits);
    return MODEL_TEXTURE_USER_DEFINED + (m->iqm->texture_unit++ % (model_totalTextureUnits - MODEL_TEXTURE_USER_DEFINED));
}

void model_applyuniform(model_t m, const model_uniform_t *t);

static
void model_set_uniforms(model_t m, int shader, mat44 mv, mat44 proj, mat44 view, mat44 model) {
    if(!m.iqm) return;
    iqm_t *q = m.iqm;

    int slot = shader == q->shadow_program ? 1 : shader == q->material_program ? 2 : 0;
    if (q->uniforms[slot][MODEL_UNIFORM_MODEL] == -1) {
        model_init_uniforms(q, slot, shader);
    }

    shader_bind(shader);
    int loc;

    if ((loc = q->uniforms[slot][MODEL_UNIFORM_MV]) >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, mv);
    }
    if ((loc = q->uniforms[slot][MODEL_UNIFORM_MVP]) >= 0) {
        mat44 mvp; multiply44x2(mvp, proj, mv);
        glUniformMatrix4fv(loc, 1, GL_FALSE, mvp);
    }
    if ((loc = q->uniforms[slot][MODEL_UNIFORM_VP]) >= 0) {
        mat44 vp; multiply44x2(vp, proj, view);
        glUniformMatrix4fv(loc, 1, GL_FALSE, vp);
    }
    if ((loc = q->uniforms[slot][MODEL_UNIFORM_CAM_POS]) >= 0) {
        vec3 pos = pos44(view);
        glUniform3fv(loc, 1, &pos.x);
    }
    if ((loc = q->uniforms[slot][MODEL_UNIFORM_CAM_DIR]) >= 0) {
        vec3 dir = norm3(vec3(view[2], view[6], view[10]));
        glUniform3fv(loc, 1, &dir.x);
    }
    if ((loc = q->uniforms[slot][MODEL_UNIFORM_BILLBOARD]) >= 0) {
        glUniform1i(loc, m.billboard);
    }
    if ((loc = q->uniforms[slot][MODEL_UNIFORM_MODEL]) >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, model);
    }
    if ((loc = q->uniforms[slot][MODEL_UNIFORM_VIEW]) >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, view);
    }
    if ((loc = q->uniforms[slot][MODEL_UNIFORM_INV_VIEW]) >= 0) {
        mat44 inv_view;
        invert44(inv_view, view);
        glUniformMatrix4fv(loc, 1, GL_FALSE, inv_view);
    }
    if ((loc = q->uniforms[slot][MODEL_UNIFORM_PROJ]) >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, proj);
    }
    if ((loc = q->uniforms[slot][MODEL_UNIFORM_SKINNED]) >= 0) {
        glUniform1i(loc, q->numanims ? GL_TRUE : GL_FALSE);
    }
    if ((loc = q->uniforms[slot][MODEL_UNIFORM_INSTANCED]) >= 0) {
        glUniform1i(loc, m.num_instances > 1 ? GL_TRUE : GL_FALSE);
    }
    if (q->numanims) {
        if ((loc = q->uniforms[slot][MODEL_UNIFORM_VS_BONE_MATRIX]) >= 0) {
            glUniformMatrix3x4fv(loc, q->numjoints, GL_FALSE, q->outframe[0]);
        }
    }
    
    if ((loc = q->uniforms[slot][MODEL_UNIFORM_FRAME_COUNT]) >= 0) {
        glUniform1i(loc, (unsigned)window_frame());
    }
    if ((loc = q->uniforms[slot][MODEL_UNIFORM_FRAME_TIME]) >= 0) {
        glUniform1f(loc, (float)window_time());
    }

    // Shadow uniforms
    if (shader == q->shadow_program) {
        shadowmap_t *sm = active_shadowmap;
        ASSERT(sm);
        if ((loc = q->uniforms[slot][MODEL_UNIFORM_SHADOW_CAMERA_TO_SHADOW_VIEW]) >= 0) {
            glUniformMatrix4fv(loc, 1, GL_FALSE, sm->V);
        }
        if ((loc = q->uniforms[slot][MODEL_UNIFORM_SHADOW_CAMERA_TO_SHADOW_PROJECTOR]) >= 0) {
            glUniformMatrix4fv(loc, 1, GL_FALSE, sm->PV);
        }
        if ((loc = q->uniforms[slot][MODEL_UNIFORM_SHADOW_TECHNIQUE]) >= 0) {
            glUniform1i(loc, sm->shadow_technique);
        }
    } else {
        // Shadow receiving
        if (m.shadow_map && m.shadow_receiver) {
            if ((loc = q->uniforms[slot][MODEL_UNIFORM_U_SHADOW_RECEIVER]) >= 0) {
                glUniform1i(loc, GL_TRUE);
            }
            if ((loc = q->uniforms[slot][MODEL_UNIFORM_U_BLEND_REGION]) >= 0) {
                glUniform1f(loc, m.shadow_map->blend_region);
            }
            if ((loc = q->uniforms[slot][MODEL_UNIFORM_MAX_SHADOW_CASCADES]) >= 0) {
                glUniform1i(loc, m.shadow_map->max_cascades);
            }

            bool was_csm_pushed = 0;
            int shadow_lights_pushed = 0;
            for (int i = 0; i < MAX_LIGHTS; i++) {
                if (shadow_lights_pushed >= MAX_SHADOW_LIGHTS) break;
                shadow_lights_pushed++;
                if (i < m.lights.count) {
                    light_t *light = &m.lights.base[i];

                    if (light->shadow_technique == SHADOW_CSM) {
                        for (int j = 0; j < NUM_SHADOW_CASCADES; j++) {
                            shader_texture_unit_kind_(GL_TEXTURE_2D, q->uniforms[slot][MODEL_UNIFORM_SHADOW_MAP_2D + j], m.shadow_map->maps[i].texture_2d[j], MODEL_TEXTURE_SHADOW_MAP_2D + j);
                            glUniform1f(q->uniforms[slot][MODEL_UNIFORM_SHADOW_CASCADE_DISTANCES + j], m.shadow_map->maps[i].cascade_distances[j]);
                        }
                        was_csm_pushed = 1;
                        shader_texture_unit_kind_(GL_TEXTURE_CUBE_MAP, q->uniforms[slot][MODEL_UNIFORM_SHADOW_MAP_CUBEMAP+i], 0, MODEL_TEXTURE_SHADOW_MAP_CUBEMAP+i);
                    }
                    else if (light->shadow_technique == SHADOW_VSM) {
                        shader_texture_unit_kind_(GL_TEXTURE_CUBE_MAP, q->uniforms[slot][MODEL_UNIFORM_SHADOW_MAP_CUBEMAP+i], m.shadow_map->maps[i].texture, MODEL_TEXTURE_SHADOW_MAP_CUBEMAP+i);
                    }
                } else {
                    if (!was_csm_pushed) {
                        was_csm_pushed = 1;
                        for (int j = 0; j < NUM_SHADOW_CASCADES; j++) {
                            glUniform1i(q->uniforms[slot][MODEL_UNIFORM_SHADOW_MAP_2D + j], MODEL_TEXTURE_SHADOW_MAP_2D + j);
                        }
                    }
                    glUniform1i(q->uniforms[slot][MODEL_UNIFORM_SHADOW_MAP_CUBEMAP+i], MODEL_TEXTURE_SHADOW_MAP_CUBEMAP+i);
                }
            }
            if ((loc = q->uniforms[slot][MODEL_UNIFORM_SHADOW_OFFSETS]) >= 0) {
                shader_texture_unit_kind_(GL_TEXTURE_3D, q->uniforms[slot][MODEL_UNIFORM_SHADOW_OFFSETS], m.shadow_map->offsets_texture, MODEL_TEXTURE_SHADOW_OFFSETS);
            }
            if ((loc = q->uniforms[slot][MODEL_UNIFORM_SHADOW_FILTER_SIZE]) >= 0) {
                glUniform1i(loc, m.shadow_map->filter_size);
            }
            if ((loc = q->uniforms[slot][MODEL_UNIFORM_SHADOW_WINDOW_SIZE]) >= 0) {
                glUniform1i(loc, m.shadow_map->window_size);
            }
        }
        else if (m.shadow_map == NULL || !m.shadow_receiver) {
            if ((loc = q->uniforms[slot][MODEL_UNIFORM_U_SHADOW_RECEIVER]) >= 0) {
                glUniform1i(loc, GL_FALSE);
            }
            for (int j = 0; j < NUM_SHADOW_CASCADES; j++) {
                if ((loc = q->uniforms[slot][MODEL_UNIFORM_SHADOW_MAP_2D + j]) >= 0) {
                    glUniform1i(loc, MODEL_TEXTURE_SHADOW_MAP_2D + j);
                }
            }
            for (int i = 0; i < MAX_SHADOW_LIGHTS; i++) {
                if ((loc = q->uniforms[slot][MODEL_UNIFORM_SHADOW_MAP_CUBEMAP + i]) >= 0) {
                    glUniform1i(loc, MODEL_TEXTURE_SHADOW_MAP_CUBEMAP + i);
                }
            }
            if ((loc = q->uniforms[slot][MODEL_UNIFORM_SHADOW_OFFSETS]) >= 0) {
                glUniform1i(q->uniforms[slot][MODEL_UNIFORM_SHADOW_OFFSETS], MODEL_TEXTURE_SHADOW_OFFSETS);
            }
        }
    }

    if ((loc = q->uniforms[slot][MODEL_UNIFORM_RESOLUTION]) >= 0) {
        glUniform2f(loc, (float)window_width(), (float)window_height());
    }
    
    bool has_tex_skysphere = m.sky_refl.id != 0;
    bool has_tex_skyenv = m.sky_env.id != 0;
    bool has_tex_env_cubemap = m.sky_cubemap.id != 0;
    glUniform1i(q->uniforms[slot][MODEL_UNIFORM_HAS_TEX_ENV_CUBEMAP], has_tex_env_cubemap);
    glUniform1i(q->uniforms[slot][MODEL_UNIFORM_HAS_TEX_SKYSPHERE], has_tex_skysphere);
    glUniform1i(q->uniforms[slot][MODEL_UNIFORM_HAS_TEX_SKYENV], has_tex_skyenv);
    if (has_tex_env_cubemap) {
        shader_texture_unit_kind_(GL_TEXTURE_CUBE_MAP, q->uniforms[slot][MODEL_UNIFORM_TEX_ENV_CUBEMAP], m.sky_cubemap.id, MODEL_TEXTURE_ENV_CUBEMAP);
    } else {
        glUniform1i(q->uniforms[slot][MODEL_UNIFORM_TEX_ENV_CUBEMAP], MODEL_TEXTURE_ENV_CUBEMAP);
    }
    if( has_tex_skysphere ) {
        float mipCount = floor( log2( max(m.sky_refl.w, m.sky_refl.h) ) );
        shader_texture_unit_kind_(GL_TEXTURE_2D, q->uniforms[slot][MODEL_UNIFORM_TEX_SKYSPHERE], m.sky_refl.id, MODEL_TEXTURE_SKYSPHERE);
        glUniform1f(q->uniforms[slot][MODEL_UNIFORM_SKYSPHERE_MIP_COUNT], mipCount);
    } else {
        glUniform1i(q->uniforms[slot][MODEL_UNIFORM_TEX_SKYSPHERE], MODEL_TEXTURE_SKYSPHERE);
    }
    if( has_tex_skyenv ) {
        shader_texture_unit_kind_(GL_TEXTURE_2D, q->uniforms[slot][MODEL_UNIFORM_TEX_SKYENV], m.sky_env.id, MODEL_TEXTURE_SKYENV);
    } else {
        glUniform1i(q->uniforms[slot][MODEL_UNIFORM_TEX_SKYENV], MODEL_TEXTURE_SKYENV);
    }
    shader_texture_unit_kind_(GL_TEXTURE_2D, q->uniforms[slot][MODEL_UNIFORM_TEX_BRDF_LUT], brdf_lut().id, MODEL_TEXTURE_BRDF_LUT);

    /* apply custom model uniforms */
    for (int i = 0; i < array_count(m.uniforms); i++) {
        model_applyuniform(m, &m.uniforms[i]);
    }
}

static inline
uint32_t model_instancing_checksum(float *matrices, unsigned count) {
    uint32_t checksum = 0;
    float *data = matrices;
    int total_floats = count * 16;

    for (int i = 0; i < total_floats; i++) {
        uint32_t int_value = *(uint32_t*)&data[i];
        checksum = ((checksum << 5) + checksum) + int_value;
    }

    return checksum;
}

static
void model_set_state(model_t m) {
    if(!m.iqm) return;
    iqm_t *q = m.iqm;

    glBindVertexArray( q->vao );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, q->ibo);
    glBindBuffer(GL_ARRAY_BUFFER, q->vbo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(iqm_vertex), (GLvoid*)offsetof(iqm_vertex, position) );
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(iqm_vertex), (GLvoid*)offsetof(iqm_vertex, texcoord) );
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(iqm_vertex), (GLvoid*)offsetof(iqm_vertex, normal) );
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(iqm_vertex), (GLvoid*)offsetof(iqm_vertex, tangent) );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    // vertex color
    glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(iqm_vertex), (GLvoid*)offsetof(iqm_vertex,color) );
    glEnableVertexAttribArray(11);

    // lmap data
    glVertexAttribPointer(12, 2, GL_FLOAT, GL_FALSE, sizeof(iqm_vertex), (GLvoid*)offsetof(iqm_vertex, texcoord2) );
    glEnableVertexAttribArray(12);

    // animation
    if(q->numframes > 0) {
        glVertexAttribPointer( 8, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(iqm_vertex), (GLvoid*)offsetof(iqm_vertex,blendindexes) );
        glVertexAttribPointer( 9, 4, GL_UNSIGNED_BYTE, GL_TRUE,  sizeof(iqm_vertex), (GLvoid*)offsetof(iqm_vertex,blendweights) );
        glVertexAttribPointer(10, 1, GL_FLOAT, GL_FALSE, sizeof(iqm_vertex), (GLvoid*)offsetof(iqm_vertex, blendvertexindex) );
        glEnableVertexAttribArray(8);
        glEnableVertexAttribArray(9);
        glEnableVertexAttribArray(10);
    }

    // mat4 attribute; for instanced rendering
    if(m.num_instances > 1) {
        unsigned vec4_size = sizeof(vec4);
        unsigned mat4_size = sizeof(vec4) * 4;

        // vertex buffer object
        glBindBuffer(GL_ARRAY_BUFFER, m.vao_instanced);

        uint32_t checksum = model_instancing_checksum(m.instanced_matrices, m.num_instances);
        if (checksum != q->instancing_checksum) {
            q->instancing_checksum = checksum;
            glBufferData(GL_ARRAY_BUFFER, m.num_instances * mat4_size, m.instanced_matrices, GL_STREAM_DRAW);
        }

        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4_size, (GLvoid*)(((char*)NULL)+(0 * vec4_size)));
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4_size, (GLvoid*)(((char*)NULL)+(1 * vec4_size)));
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4_size, (GLvoid*)(((char*)NULL)+(2 * vec4_size)));
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4_size, (GLvoid*)(((char*)NULL)+(3 * vec4_size)));

        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
        glEnableVertexAttribArray(6);
        glEnableVertexAttribArray(7);

        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glVertexAttribDivisor(7, 1);
    }

    // 7 bitangent? into texcoord.z?

    glBindVertexArray( 0 );
}

void model_sync(model_t m, int num_vertices, model_vertex_t *vertices, int num_indices, uint32_t *indices) {
    if (!m.iqm) return;
    iqm_t *q = m.iqm;

    if (!(m.flags & MODEL_PROCEDURAL)) {
        die("model_sync() cannot be used with non-procedural models");
    }

    if (!q->vao) glGenVertexArrays(1, &q->vao);
    glBindVertexArray(q->vao);
   
    if (num_vertices > 0) { 
        if(!q->vbo) glGenBuffers(1, &q->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, q->vbo);
        glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(model_vertex_t), vertices, m.flags & MODEL_STREAM ? GL_STREAM_DRAW : GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (num_indices > 0) {
        if (!q->ibo) glGenBuffers(1, &q->ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, q->ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(uint32_t), indices, m.flags & MODEL_STREAM ? GL_STREAM_DRAW : GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    q->nummeshes = 1;
    q->numtris = num_indices/3;
    q->numverts = num_vertices;

    if (!q->meshes) q->meshes = CALLOC(q->nummeshes, sizeof(struct iqmmesh));
    struct iqmmesh *mesh = &q->meshes[0];
    mesh->first_vertex = 0;
    mesh->num_vertexes = num_vertices;
    mesh->first_triangle = 0;
    mesh->num_triangles = num_indices/3;

    model_set_state(m);
    glBindVertexArray(0);
}

static
bool model_load_meshes(iqm_t *q, const struct iqmheader *hdr, model_t *m) {
    if(q->meshdata) return false;

    lil32p(&q->buf[hdr->ofs_vertexarrays], hdr->num_vertexarrays*sizeof(struct iqmvertexarray)/sizeof(uint32_t));
    lil32p(&q->buf[hdr->ofs_triangles], hdr->num_triangles*sizeof(struct iqmtriangle)/sizeof(uint32_t));
    lil32p(&q->buf[hdr->ofs_meshes], hdr->num_meshes*sizeof(struct iqmmesh)/sizeof(uint32_t));
    lil32p(&q->buf[hdr->ofs_joints], hdr->num_joints*sizeof(struct iqmjoint)/sizeof(uint32_t));

    q->meshdata = q->buf;
    q->nummeshes = hdr->num_meshes;
    q->numtris = hdr->num_triangles;
    q->numverts = hdr->num_vertexes;
    q->numjoints = hdr->num_joints;
    q->numframes = hdr->num_frames;
    q->outframe = CALLOC(hdr->num_joints, sizeof(mat34));

    float *inposition = NULL, *innormal = NULL, *intangent = NULL, *intexcoord = NULL, *invertexindex = NULL;
    uint8_t *inblendindex8 = NULL, *inblendweight8 = NULL;
    int *inblendindexi = NULL; float *inblendweightf = NULL;
    float *invertexcolor = NULL;
    struct iqmvertexarray *vs = (struct iqmvertexarray *)&q->buf[hdr->ofs_vertexarrays];
    for(int i = 0; i < (int)hdr->num_vertexarrays; i++) {
        struct iqmvertexarray *v = &vs[i];
        switch(v->type) {
        default: continue; // return die("unknown iqm vertex type (%d)", v->type), false;
        break; case IQM_POSITION: ASSERT(v->format == IQM_FLOAT && v->size == 3); inposition = (float *)&q->buf[v->offset]; lil32pf(inposition, 3*hdr->num_vertexes);
        break; case IQM_NORMAL: ASSERT(v->format == IQM_FLOAT && v->size == 3); innormal = (float *)&q->buf[v->offset]; lil32pf(innormal, 3*hdr->num_vertexes);
        break; case IQM_TANGENT: ASSERT(v->format == IQM_FLOAT && v->size == 4); intangent = (float *)&q->buf[v->offset]; lil32pf(intangent, 4*hdr->num_vertexes);
        break; case IQM_TEXCOORD: ASSERT(v->format == IQM_FLOAT && v->size == 2); intexcoord = (float *)&q->buf[v->offset]; lil32pf(intexcoord, 2*hdr->num_vertexes);
        break; case IQM_COLOR: ASSERT(v->size == 4); ASSERT(v->format == IQM_FLOAT); invertexcolor = (float *)&q->buf[v->offset];
        break; case IQM_BLENDINDEXES: ASSERT(v->size == 4); ASSERT(v->format == IQM_UBYTE || v->format == IQM_INT);
        if(v->format == IQM_UBYTE) inblendindex8 = (uint8_t *)&q->buf[v->offset];
        else inblendindexi = (int *)&q->buf[v->offset];
        break; case IQM_BLENDWEIGHTS: ASSERT(v->size == 4); ASSERT(v->format == IQM_UBYTE || v->format == IQM_FLOAT);
        if(v->format == IQM_UBYTE) inblendweight8 = (uint8_t *)&q->buf[v->offset];
        else inblendweightf = (float *)&q->buf[v->offset];
        invertexindex = (inblendweight8 ? (float*)(inblendweight8 + 4) : inblendweightf + 4 );
        }
    }

    if (hdr->ofs_bounds) lil32p(q->buf + hdr->ofs_bounds, hdr->num_frames * sizeof(struct iqmbounds));
    if (hdr->ofs_bounds) q->bounds = (struct iqmbounds *) &q->buf[hdr->ofs_bounds];

    q->meshes = (struct iqmmesh *)&q->buf[hdr->ofs_meshes];
    q->joints = (struct iqmjoint *)&q->buf[hdr->ofs_joints];

    q->baseframe = CALLOC(hdr->num_joints, sizeof(mat34));
    q->inversebaseframe = CALLOC(hdr->num_joints, sizeof(mat34));
    for(int i = 0; i < (int)hdr->num_joints; i++) {
        struct iqmjoint *j = &q->joints[i];
        compose34(q->baseframe[i], ptr3(j->translate), normq(ptrq(j->rotate)), ptr3(j->scale));
        invert34(q->inversebaseframe[i], q->baseframe[i]);
        if(j->parent >= 0) {
            multiply34x2(q->baseframe[i], q->baseframe[j->parent], q->baseframe[i]);
            multiply34(q->inversebaseframe[i], q->inversebaseframe[j->parent]);
        }
    }

    struct iqmtriangle *tris = (struct iqmtriangle *)&q->buf[hdr->ofs_triangles];
    m->num_tris = hdr->num_triangles;
    m->tris = (void*)tris;

    glGenVertexArrays(1, &q->vao);
    glBindVertexArray(q->vao);

    if(!q->ibo) glGenBuffers(1, &q->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, q->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, hdr->num_triangles*sizeof(struct iqmtriangle), tris, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    iqm_vertex *verts = CALLOC(hdr->num_vertexes, sizeof(iqm_vertex));
    for(int i = 0; i < (int)hdr->num_vertexes; i++) {
        iqm_vertex *v = &verts[i];
        if(inposition) memcpy(v->position, &inposition[i*3], sizeof(v->position));
        if(innormal) memcpy(v->normal, &innormal[i*3], sizeof(v->normal));
        if(intangent) memcpy(v->tangent, &intangent[i*4], sizeof(v->tangent));
        if(intexcoord) {
            memcpy(v->texcoord, &intexcoord[i*2], sizeof(v->texcoord));
            memcpy(v->texcoord2, &intexcoord[i*2], sizeof(v->texcoord2)); // populate UV1 with the same value, used by lightmapper
        }
        if(inblendindex8) memcpy(v->blendindexes, &inblendindex8[i*4], sizeof(v->blendindexes));
        if(inblendweight8) memcpy(v->blendweights, &inblendweight8[i*4], sizeof(v->blendweights));
        if(inblendindexi) {
            uint8_t conv[4] = { inblendindexi[i*4], inblendindexi[i*4+1], inblendindexi[i*4+2], inblendindexi[i*4+3] };
            memcpy(v->blendindexes, conv, sizeof(v->blendindexes));
        }
        if(inblendweightf) {
            uint8_t conv[4] = { inblendweightf[i*4] * 255, inblendweightf[i*4+1] * 255, inblendweightf[i*4+2] * 255, inblendweightf[i*4+3] * 255 };
            memcpy(v->blendweights, conv, sizeof(v->blendweights));
        }
        if(invertexindex) {
            float conv = i;
            memcpy(&v->blendvertexindex, &conv, 4);
        }
        if(invertexcolor) {
            v->color[0] = invertexcolor[i*4+0];
            v->color[1] = invertexcolor[i*4+1];
            v->color[2] = invertexcolor[i*4+2];
            v->color[3] = invertexcolor[i*4+3];
        }
        else {
            v->color[0] = 1.0f;
            v->color[1] = 1.0f;
            v->color[2] = 1.0f;
            v->color[3] = 1.0f;
        }

        /* handle vertex colors for parts of mesh that don't utilise it. */
        if (v->color[0] + v->color[1] + v->color[2] + v->color[3] < 0.001f) {
            v->color[0] = 1.0f;
            v->color[1] = 1.0f;
            v->color[2] = 1.0f;
            v->color[3] = 1.0f;
        }
    }

    if(!q->vbo) glGenBuffers(1, &q->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, q->vbo);
    glBufferData(GL_ARRAY_BUFFER, hdr->num_vertexes*sizeof(iqm_vertex), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m->stride = sizeof(iqm_vertex);
    #if 0
    m->stride = 0;
    if(inposition) m->stride += sizeof(verts[0].position);
    if(innormal) m->stride += sizeof(verts[0].normal);
    if(intangent) m->stride += sizeof(verts[0].tangent);
    if(intexcoord) m->stride += sizeof(verts[0].texcoord);
    if(inblendindex8) m->stride += sizeof(verts[0].blendindexes); // no index8? bug?
    if(inblendweight8) m->stride += sizeof(verts[0].blendweights); // no weight8? bug?
    if(inblendindexi) m->stride += sizeof(verts[0].blendindexes);
    if(inblendweightf) m->stride += sizeof(verts[0].blendweights);
    if(invertexcolor8) m->stride += sizeof(verts[0].color);
    #endif
    //for( int i = 0; i < 16; ++i ) printf("%.9g%s", ((float*)verts)[i], (i % 3) == 2 ? "\n" : ",");
    m->verts = verts;
    /*m->verts = 0; FREE(verts);*/

    q->mesh_materials = CALLOC(hdr->num_meshes, sizeof(unsigned));

    const char *str = hdr->ofs_text ? (char *)&q->buf[hdr->ofs_text] : "";
    for(int i = 0; i < (int)hdr->num_meshes; i++) {
        struct iqmmesh *m = &q->meshes[i];
        PRINTF("loaded mesh: %s\n", &str[m->name]);
    }

    return true;
}

static
bool model_load_anims(iqm_t *q, const struct iqmheader *hdr) {
    if((int)hdr->num_poses != q->numjoints) return false;

    if(q->animdata) {
        if(q->animdata != q->meshdata) FREE(q->animdata);
        FREE(q->frames);
        q->animdata = NULL;
        q->anims = NULL;
        q->frames = 0;
        q->numframes = 0;
        q->numanims = 0;
    }

    lil32p(&q->buf[hdr->ofs_poses], hdr->num_poses*sizeof(struct iqmpose)/sizeof(uint32_t));
    lil32p(&q->buf[hdr->ofs_anims], hdr->num_anims*sizeof(struct iqmanim)/sizeof(uint32_t));
    lil16p((uint16_t *)&q->buf[hdr->ofs_frames], hdr->num_frames*hdr->num_framechannels);

    q->animdata = q->buf;
    q->numanims = hdr->num_anims;
    q->numframes = hdr->num_frames;

    q->anims = (struct iqmanim *)&q->buf[hdr->ofs_anims];
    q->poses = (struct iqmpose *)&q->buf[hdr->ofs_poses];
    q->frames = CALLOC(hdr->num_frames * hdr->num_poses, sizeof(mat34));
    uint16_t *framedata = (uint16_t *)&q->buf[hdr->ofs_frames];

    for(int i = 0; i < (int)hdr->num_frames; i++) {
        for(int j = 0; j < (int)hdr->num_poses; j++) {
            struct iqmpose *p = &q->poses[j];
            quat rotate;
            vec3 translate, scale;
            translate.x = p->channeloffset[0]; if(p->mask&0x01) translate.x += *framedata++ * p->channelscale[0];
            translate.y = p->channeloffset[1]; if(p->mask&0x02) translate.y += *framedata++ * p->channelscale[1];
            translate.z = p->channeloffset[2]; if(p->mask&0x04) translate.z += *framedata++ * p->channelscale[2];

            rotate.x = p->channeloffset[3]; if(p->mask&0x08) rotate.x += *framedata++ * p->channelscale[3];
            rotate.y = p->channeloffset[4]; if(p->mask&0x10) rotate.y += *framedata++ * p->channelscale[4];
            rotate.z = p->channeloffset[5]; if(p->mask&0x20) rotate.z += *framedata++ * p->channelscale[5];
            rotate.w = p->channeloffset[6]; if(p->mask&0x40) rotate.w += *framedata++ * p->channelscale[6];

            scale.x = p->channeloffset[7]; if(p->mask&0x80)  scale.x += *framedata++ * p->channelscale[7];
            scale.y = p->channeloffset[8]; if(p->mask&0x100) scale.y += *framedata++ * p->channelscale[8];
            scale.z = p->channeloffset[9]; if(p->mask&0x200) scale.z += *framedata++ * p->channelscale[9];

            // Concatenate each pose with the inverse base pose to avoid doing this at animation time.
            // If the joint has a parent, then it needs to be pre-concatenated with its parent's base pose.
            // Thus it all negates at animation time like so:
            //   (parentPose * parentInverseBasePose) * (parentBasePose * childPose * childInverseBasePose) =>
            //   parentPose * (parentInverseBasePose * parentBasePose) * childPose * childInverseBasePose =>
            //   parentPose * childPose * childInverseBasePose

            mat34 m; compose34(m, translate, normq(rotate), scale);
            if(p->parent >= 0) multiply34x3(q->frames[i*hdr->num_poses + j], q->baseframe[p->parent], m, q->inversebaseframe[j]);
            else multiply34x2(q->frames[i*hdr->num_poses + j], m, q->inversebaseframe[j]);
        }
    }

    // const char *str = hdr->ofs_text ? (char *)&q->buf[hdr->ofs_text] : "";
    // for(int i = 0; i < (int)hdr->num_anims; i++) {
    //     struct iqmanim *a = &anims[i];
    //     PRINTF("loaded anim[%d]: %s\n", i, &str[a->name]);
    // }

    return true;
}

void ui_material(material_t *m) {
    ASSERT(m);
    if (!m->_loaded) return;

    ui_bool("Disable IBL", &m->disable_ibl);
    ui_float("SSR Strength", &m->ssr_strength);
    ui_bool("Parallax Clip", &m->parallax_clip);
    ui_float("Cutout Alpha", &m->cutout_alpha);
    ui_separator();

    static char* channel_names[] = {
        "Albedo", "Normals", "Roughness", "Metallic", "AO", "Ambient", "Emissive", "Parallax"
    };
    for (int i = 0; i < MAX_CHANNELS_PER_MATERIAL; i++) {
        
        if (ui_collapse(channel_names[i], va("%s_%d", m->name, i))) {
            ui_color4f(va("%s Color", channel_names[i]), &m->layer[i].map.color.x);
            
            if (m->layer[i].map.texture) {
                ui_texture(va("%s Texture", channel_names[i]), *m->layer[i].map.texture);
            }
            
            if (i == MATERIAL_CHANNEL_PARALLAX) {
                ui_float("Parallax Scale", &m->layer[i].value);
                ui_float("Parallax Shadow Power", &m->layer[i].value2);
            }

            if (i == MATERIAL_CHANNEL_EMISSIVE) {
                ui_float("Emissive Value", &m->layer[i].value);
            }
            
            ui_collapse_end();
        }
    }
}

void ui_materials(model_t *m) {
    for (int i = 0; i < array_count(m->materials); i++) {
        if (ui_collapse(m->materials[i].name, va("material_%d", i))) {
            ui_material(&m->materials[i]);
            ui_collapse_end();
        }
    }
}

// prevents crash on osx when strcpy'ing non __restrict arguments
static char* strcpy_safe(char *d, const char *s) {
    sprintf(d, "%s", s);
    return d;
}

static
void model_load_pbr_layer(material_layer_t *layer, const char *texname, bool load_as_srgb) {
    strcpy_safe(layer->texname, texname);
    colormap(&layer->map, texname, load_as_srgb);
}

static
void model_load_pbr(model_t *m, material_t *mt) {
    if (!m->iqm) return;
    struct iqm_t *q = m->iqm;

    if (mt->_loaded) return;
    mt->_loaded = true;

    // initialise default colors
    mt->layer[MATERIAL_CHANNEL_NORMALS].map.color = vec4(0,0,0,0);
    mt->layer[MATERIAL_CHANNEL_ROUGHNESS].map.color = vec4(1,1,1,1);
    mt->layer[MATERIAL_CHANNEL_METALLIC].map.color = vec4(0,0,0,0);
    mt->layer[MATERIAL_CHANNEL_AO].map.color = vec4(1,1,1,0);
    mt->layer[MATERIAL_CHANNEL_AMBIENT].map.color = vec4(0,0,0,1);
    mt->layer[MATERIAL_CHANNEL_EMISSIVE].map.color = vec4(0,0,0,0);
    mt->layer[MATERIAL_CHANNEL_PARALLAX].map.color = vec4(0,0,0,0);
    mt->layer[MATERIAL_CHANNEL_EMISSIVE].value = 1.0f;
    mt->layer[MATERIAL_CHANNEL_PARALLAX].value = 0.1f;
    mt->layer[MATERIAL_CHANNEL_PARALLAX].value2 = 4.0f;
    
    // load colormaps
    array(char*) tokens = strsplit(mt->name, "+");
    for( int j = 0, end = array_count(tokens); j < end; ++j ) {
        char *t = tokens[j];
        if (strstri(t, "_C.") || strstri(t, "Cutout") ) {
            mt->cutout_alpha = 0.75f;
            mt->layer[MATERIAL_CHANNEL_ALBEDO].map.no_mipmaps = true;
        }

        if( strstri(t, "_A.") || strstri(t, "Albedo") || strstri(t, "_D.") || strstri(t, "Diffuse") || strstri(t, "BaseColor") || strstri(t, "Base_Color") )     { model_load_pbr_layer(&mt->layer[MATERIAL_CHANNEL_ALBEDO], t, 1); continue; }
        else
        if( strstri(t, "_N.") || strstri(t, "Normal") )     { model_load_pbr_layer(&mt->layer[MATERIAL_CHANNEL_NORMALS], t, 0); continue; }
        else
        if( strstri(t, "_R.") || strstri(t, "Roughness") )  { model_load_pbr_layer(&mt->layer[MATERIAL_CHANNEL_ROUGHNESS], t, 0); continue; }
        else
        if( strstri(t, "_MR.")|| strstri(t, "MetallicRoughness") || strstri(t, "OcclusionRoughnessMetallic") )  { model_load_pbr_layer(&mt->layer[MATERIAL_CHANNEL_ROUGHNESS], t, 0); continue; }
        else
        if( strstri(t, "_M.") || strstri(t, "Metallic") )   { model_load_pbr_layer(&mt->layer[MATERIAL_CHANNEL_METALLIC], t, 0); continue; }
        else
        if( strstri(t, "_E.") || strstri(t, "Emissive") || strstri(t, "Emission") )   { model_load_pbr_layer(&mt->layer[MATERIAL_CHANNEL_EMISSIVE], t, 1); continue; }
        else
        if( strstri(t, "_AO.") || strstri(t, "AO") || strstri(t, "Occlusion") ) { model_load_pbr_layer(&mt->layer[MATERIAL_CHANNEL_AO], t, 0); continue; }
        else
        if( strstri(t, "_P.") || strstri(t, "Parallax") || strstri(t, "disp.") ) { model_load_pbr_layer(&mt->layer[MATERIAL_CHANNEL_PARALLAX], t, 0); continue; }
        // else
        //     { model_load_pbr_layer(&mt->layer[MATERIAL_CHANNEL_ALBEDO], t, 1); continue; }

    }
}

static
bool model_load_textures(iqm_t *q, const struct iqmheader *hdr, model_t *model, int _flags) {
    texture_t tex = texture_checker();
    GLuint out = 0;

    const char *str = hdr->ofs_text ? (char *)&q->buf[hdr->ofs_text] : "";
    for(int i = 0; i < (int)hdr->num_meshes; i++) {
        struct iqmmesh *m = &q->meshes[i];

        // reuse texture+material if already decoded
        bool reused = 0;
        for( int j = 0; !reused && j < model->num_textures; ++j ) {
            if( !strcmpi(model->texture_names[j], &str[m->material])) {
                q->mesh_materials[i] = j;
                reused = true;
                break;
            }
        }
        if( reused ) continue;

        // decode texture+material
        int flags = TEXTURE_MIPMAPS|TEXTURE_REPEAT|TEXTURE_ANISOTROPY|TEXTURE_SRGB; // LINEAR, NEAREST
        if (!(_flags & MODEL_NO_FILTERING))
            flags |= TEXTURE_LINEAR;
        int invalid = texture_checker().id;

        char *material_embedded_texture = 0;
        material_embedded_texture = strstr(&str[m->material], "+b64:");
        if( material_embedded_texture ) {
            *material_embedded_texture = '\0';
            material_embedded_texture += 5;
            array(char) embedded_texture = base64_decode(material_embedded_texture, strlen(material_embedded_texture));
            //printf("%s %d\n", material_embedded_texture, array_count(embedded_texture));
            //hexdump(embedded_texture, array_count(embedded_texture));
            tex = texture_compressed_from_mem( embedded_texture, array_count(embedded_texture), flags );
            out = tex.id;
            array_free(embedded_texture);
        }

        char* material_color_hex = strstr(&str[m->material], "+$");
        vec4 material_color = vec4(1,1,1,1);
        if( material_color_hex ) {
            *material_color_hex = '\0';
            material_color_hex += 2;
            material_color.r = ((material_color_hex[0] >= 'a') ? material_color_hex[0] - 'a' + 10 : material_color_hex[0] - '0') / 15.f;
            material_color.g = ((material_color_hex[1] >= 'a') ? material_color_hex[1] - 'a' + 10 : material_color_hex[1] - '0') / 15.f;
            material_color.b = ((material_color_hex[2] >= 'a') ? material_color_hex[2] - 'a' + 10 : material_color_hex[2] - '0') / 15.f;
            material_color.a = ((material_color_hex[3] >= 'a') ? material_color_hex[3] - 'a' + 10 : material_color_hex[3] - '0') / 15.f;
            #if 0 // not enabled because of some .obj files like suzanne, with color_hex=9990 found
            if(material_color_hex[3])
            material_color.a = ((material_color_hex[3] >= 'a') ? material_color_hex[3] - 'a' + 10 : material_color_hex[3] - '0') / 15.f;
            else
            #endif
        }

        if( !material_embedded_texture ) {
            char* material_name;
            // remove any material+name from materials (.fbx)
            // try left token first
            if( 1 ) {
                material_name = va("%s", &str[m->material]);
                char* plus = strrchr(material_name, '+');
                if (plus) { strcpy_safe(plus, file_ext(material_name)); }
                tex = texture_compressed(material_name, flags);
                out = tex.id;
            }
            // else try right token
            if (out == invalid) {
                material_name = va("%s", file_norm( &str[m->material]) );
                char* plus = strrchr(material_name, '+'), *slash = strrchr(material_name, '/');
                if (plus) {
                    strcpy_safe(slash ? slash + 1 : material_name, plus + 1);
                    tex = texture_compressed(material_name, flags);
                    out = tex.id;
                }
            }
            // else last resort
            if (out == invalid) {
                tex = texture_compressed(material_name, flags);
            }
        }

        inscribe_tex:;
        {
            model->num_textures++;
            array_push(model->texture_names, STRDUP(&str[m->material]));

            material_t mt = {0};
            mt.name = STRDUP(&str[m->material]);
            q->mesh_materials[i] = model->num_textures - 1;

            // initialise basic texture layer
            mt.layer[MATERIAL_CHANNEL_ALBEDO].map.color = material_color_hex ? material_color : vec4(1,1,1,1);
            mt.layer[MATERIAL_CHANNEL_ALBEDO].map.texture = CALLOC(1, sizeof(texture_t));
            *mt.layer[MATERIAL_CHANNEL_ALBEDO].map.texture = tex;

            array_push(model->materials, mt);
        }

    }

    if( array_count(model->materials) == 0 ) {
        material_t mt = {0};
        mt.name = "placeholder";
        mt.layer[MATERIAL_CHANNEL_ALBEDO].map.color = vec4(1,1,1,1);
        mt.layer[MATERIAL_CHANNEL_ALBEDO].map.texture = CALLOC(1, sizeof(texture_t));
        mt.layer[MATERIAL_CHANNEL_ALBEDO].map.texture->id = texture_checker().id;
        q->mesh_materials[0] = 0;

        array_push(model->materials, mt);
    }

    return true;
}

static
void model_set_renderstates(model_t *m) {
    for (int i = 0; i<NUM_RENDER_PASSES; ++i) {
        m->rs[i] = renderstate();
    }

    // Opaque pass
    renderstate_t *opaque_rs = &m->rs[RENDER_PASS_OPAQUE];
    {
        opaque_rs->blend_enabled = 0;
        opaque_rs->cull_face_mode = GL_BACK;
        opaque_rs->front_face = GL_CW;
    }

    // Transparent pass
    renderstate_t *transparent_rs = &m->rs[RENDER_PASS_TRANSPARENT];
    {
        transparent_rs->blend_enabled = 1;
        transparent_rs->blend_src = GL_SRC_ALPHA;
        transparent_rs->blend_dst = GL_ONE_MINUS_SRC_ALPHA;
        transparent_rs->blend_src_alpha = GL_SRC_ALPHA;
        transparent_rs->blend_dst_alpha = GL_ONE_MINUS_SRC_ALPHA;
        transparent_rs->cull_face_mode = GL_BACK;
        transparent_rs->front_face = GL_CW;
    }

    // Shadow pass
    renderstate_t *shadow_rs = &m->rs[RENDER_PASS_SHADOW];
    {
        shadow_rs->blend_enabled = 0;
        shadow_rs->depth_test_enabled = true;
        shadow_rs->depth_write_enabled = true;
        shadow_rs->cull_face_enabled = 0;
        shadow_rs->cull_face_mode = GL_BACK;
        shadow_rs->front_face = GL_CW;
        shadow_rs->depth_clamp_enabled = 1;
    }
}

model_t model_from_mem(const void *mem, int len, int flags) {
    model_t m = {0};

    m.stored_flags = flags;
    model_set_renderstates(&m);

    const char *ptr = (const char *)mem;
    iqm_t *q = CALLOC(1, sizeof(iqm_t));

    int error = 1;
    if( ptr && len ) {
        struct iqmheader hdr; memcpy(&hdr, ptr, sizeof(hdr)); ptr += sizeof(hdr);
        if( !memcmp(hdr.magic, IQM_MAGIC, sizeof(hdr.magic))) {
            lil32p(&hdr.version, (sizeof(hdr) - sizeof(hdr.magic))/sizeof(uint32_t));
            if(hdr.version == IQM_VERSION) {
                q->buf = CALLOC(hdr.filesize, sizeof(uint8_t));
                memcpy(q->buf + sizeof(hdr), ptr, hdr.filesize - sizeof(hdr));
                error = 0;
                if( hdr.num_meshes > 0 && !(flags & MODEL_NO_MESHES) )     error |= !model_load_meshes(q, &hdr, &m);
                if( hdr.num_meshes > 0 && !(flags & MODEL_NO_TEXTURES) )   error |= !model_load_textures(q, &hdr, &m, flags);
                else {
                    // setup fallback
                    material_t mt = {0};
                    mt.name = "placeholder";
                    mt.layer[0].map.color = vec4(1,1,1,1);
                    mt.layer[0].map.texture = CALLOC(1, sizeof(texture_t));
                    mt.layer[0].map.texture->id = texture_checker().id;

                    array_push(m.materials, mt);
                }
                if( hdr.num_anims  > 0 && !(flags & MODEL_NO_ANIMATIONS) ) error |= !model_load_anims(q, &hdr);
                if( q->buf != q->meshdata && q->buf != q->animdata ) FREE(q->buf);
            }
        }
    }

    if (flags & MODEL_PROCEDURAL) {
        error = 0;

        material_t mt = {0};
        mt.name = "base";
        mt.layer[0].map.color = vec4(1,1,1,1);
        mt.layer[0].map.texture = CALLOC(1, sizeof(texture_t));
        mt.layer[0].map.texture->id = texture_checker().id;

        q->nummeshes = 1;
        if (!q->mesh_materials) q->mesh_materials = CALLOC(1, sizeof(unsigned));

        array_push(m.materials, mt);
    }

    if(error) {
        PRINTF("Error: cannot load %s", "model");
        FREE(q), q = 0;
    } else {
        m.vao = q->vao;
        m.ibo = q->ibo;
        m.vbo = q->vbo;
        m.num_verts = q->numverts;

        // m.boxes = bounds; // <@todo
        m.num_meshes = q->nummeshes;
        m.num_triangles = q->numtris;
        m.num_joints = q->numjoints;
        //m.num_poses = numposes;
        m.num_anims = q->numanims;
        m.num_frames = q->numframes;
        m.iqm = q;
        m.curframe = model_animate(m, 0);

        //m.num_textures = q->nummeshes; // assume 1 texture only per mesh
        m.sky_env.id = 0;
        m.sky_refl.id = 0;

        m.flags = flags;

        id44(m.pivot);

        m.num_instances = 0;
        m.instanced_matrices = m.pivot;

        glGenBuffers(1, &m.vao_instanced);
        model_set_state(m);
        model_setshader(&m, NULL, NULL, NULL);

        if (m.flags & MODEL_NO_FILTERING) {
            for (int i = 0; i < array_count(m.materials); i++) {
                material_texparams(&m.materials[i], TEXTURE_NEAREST|TEXTURE_REPEAT);
            }
        }
    }
    return m;
}

array(model_t) model_cache;

static inline
void model_duplicate_mesh(model_t *m) {
    struct iqm_t *q = m->iqm;

    // duplicate mesh data
    void *old_verts = m->verts;
    m->verts = MALLOC(q->numverts * sizeof(iqm_vertex));
    memcpy(m->verts, old_verts, q->numverts * sizeof(iqm_vertex));

    void *old_tris = m->tris;
    m->tris = MALLOC(q->numtris * sizeof(struct iqmtriangle));
    memcpy(m->tris, old_tris, q->numtris * sizeof(struct iqmtriangle));

    glGenVertexArrays(1, &m->vao);
    glBindVertexArray(m->vao);

    glGenBuffers(1, &q->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, q->vbo);
    glBufferData(GL_ARRAY_BUFFER, q->numverts * sizeof(iqm_vertex), m->verts, GL_STATIC_DRAW);

    glGenBuffers(1, &q->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, q->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, q->numtris * sizeof(struct iqmtriangle), m->tris, GL_STATIC_DRAW);

    glGenBuffers(1, &m->vao_instanced);
    
    // duplicate iqm_t
    struct iqm_t *old_iqm = m->iqm;
    q = m->iqm = CALLOC(1, sizeof(iqm_t));
    memcpy(q, old_iqm, sizeof(struct iqm_t));
    q->instancing_checksum = 0; // reset checksum so the next draw call will update the buffer
    q->light_ubo = 0;

    {
        if (old_iqm->buf) {
            q->buf = MALLOC(old_iqm->nummeshes * sizeof(uint8_t));
            memcpy(q->buf, old_iqm->buf, old_iqm->nummeshes * sizeof(uint8_t));
        }
        if (old_iqm->meshdata) {
            q->meshdata = MALLOC(old_iqm->nummeshes * sizeof(uint8_t));
            memcpy(q->meshdata, old_iqm->meshdata, old_iqm->nummeshes * sizeof(uint8_t));
        }
        if (old_iqm->animdata) {
            q->animdata = MALLOC(old_iqm->nummeshes * sizeof(uint8_t));
            memcpy(q->animdata, old_iqm->animdata, old_iqm->nummeshes * sizeof(uint8_t));
        }
        if (old_iqm->mesh_materials) {
            q->mesh_materials = MALLOC(old_iqm->nummeshes * sizeof(unsigned));
            memcpy(q->mesh_materials, old_iqm->mesh_materials, old_iqm->nummeshes * sizeof(unsigned));
        }
        if (old_iqm->meshes) {
            q->meshes = MALLOC(old_iqm->nummeshes * sizeof(struct iqmmesh));
            memcpy(q->meshes, old_iqm->meshes, old_iqm->nummeshes * sizeof(struct iqmmesh));
        }
        if (old_iqm->joints) {
            q->joints = MALLOC(old_iqm->numjoints * sizeof(struct iqmjoint));
            memcpy(q->joints, old_iqm->joints, old_iqm->numjoints * sizeof(struct iqmjoint));
        }
        if (old_iqm->poses) {
            q->poses = MALLOC(max(1, old_iqm->numframes) * sizeof(struct iqmpose));
            memcpy(q->poses, old_iqm->poses, max(1, old_iqm->numframes) * sizeof(struct iqmpose));
        }
        if (old_iqm->anims) {
            q->anims = MALLOC(old_iqm->numanims * sizeof(struct iqmanim));
            memcpy(q->anims, old_iqm->anims, old_iqm->numanims * sizeof(struct iqmanim));
        }
        if (old_iqm->bounds) {
            q->bounds = MALLOC(max(1, old_iqm->numframes) * sizeof(struct iqmbounds));
            memcpy(q->bounds, old_iqm->bounds, max(1, old_iqm->numframes) * sizeof(struct iqmbounds));
        }
        if (old_iqm->baseframe) {
            q->baseframe = MALLOC(old_iqm->numjoints * sizeof(mat34));
            memcpy(q->baseframe, old_iqm->baseframe, old_iqm->numjoints * sizeof(mat34));
        }
        if (old_iqm->inversebaseframe) {
            q->inversebaseframe = MALLOC(old_iqm->numjoints * sizeof(mat34));
            memcpy(q->inversebaseframe, old_iqm->inversebaseframe, old_iqm->numjoints * sizeof(mat34));
        }
        if (old_iqm->outframe) {
            q->outframe = MALLOC(old_iqm->numjoints * sizeof(mat34));
            memcpy(q->outframe, old_iqm->outframe, old_iqm->numjoints * sizeof(mat34));
        }
        if (old_iqm->frames) {
            q->frames = MALLOC(max(1, old_iqm->numframes) * old_iqm->numjoints * sizeof(mat34));
            memcpy(q->frames, old_iqm->frames, max(1, old_iqm->numframes) * old_iqm->numjoints * sizeof(mat34));
        }
    }

    model_setshader(m, m->shaderinfo.vs ? STRDUP(m->shaderinfo.vs) : NULL, m->shaderinfo.fs ? STRDUP(m->shaderinfo.fs) : NULL, m->shaderinfo.defines ? STRDUP(m->shaderinfo.defines) : NULL);
}

static inline
void model_duplicate_materials(model_t *m) {
    material_t *old_materials = m->materials;
    m->materials = 0;
    for (int i = 0; i < array_count(old_materials); i++) {
        material_t new_mt = old_materials[i];
        new_mt.name = STRDUP(old_materials[i].name);
        array_push(m->materials, new_mt);
    }

    model_uniform_t *old_uniforms = m->uniforms;
    m->uniforms = 0;
    for (int i = 0; i < array_count(old_uniforms); i++) {
        array_push(m->uniforms, old_uniforms[i]);
    }
}

model_t model(const char *filename, int flags) {
    if (!filename) {
        return model_from_mem( NULL, 0, flags|MODEL_PROCEDURAL);
    }
    model_t *m = NULL;
    for (int i = 0; i < array_count(model_cache); i++) {
        if (strcmp(model_cache[i].filename, filename) == 0) {
            m = &model_cache[i];
            break;
        }
    }
    if (m) {
        if (flags & MODEL_SHARED) return *m;
        if (flags & MODEL_UNIQUE) {
            model_duplicate_mesh(m);
        }

        model_duplicate_materials(m);
        return *m;
    } else {
        int len;  // vfs_pushd(filedir(filename))
        char *ptr = file_read(filename, &len); // + vfs_popd
        model_t new_model = model_from_mem( ptr, len, flags );
        new_model.filename = STRDUP(filename);
        if (!(flags & MODEL_UNIQUE)) {
            array_push(model_cache, new_model);
        }
        return new_model;
    }
}

void material_texparams(material_t *m, unsigned texture_flags) {
    for (int i = 0; i < MAX_CHANNELS_PER_MATERIAL; i++) {
        if (m->layer[i].map.texture) {
            texture_params(m->layer[i].map.texture, texture_flags);
        }
    }
}

uint32_t material_checksum(material_t *m) {
    uint32_t checksum = 0;
    for (int i = 0; i < MAX_CHANNELS_PER_MATERIAL; i++) {
        checksum ^= hh_float(m->layer[i].value);
        checksum ^= (m->layer[i].map.texture) ? m->layer[i].map.texture->id : 0;
        checksum ^= hh_vec4(m->layer[i].map.color);
    }
    return checksum;
}

uint32_t model_checksum(model_t *m) {
    uint32_t checksum = 0;
    
    // Basic properties
    checksum ^= m->num_meshes ^ m->num_triangles ^ m->num_joints ^ m->num_anims ^ m->num_frames;
    checksum ^= m->flags ^ (m->shadow_receiver << 1) ^ (m->billboard << 2);
    
    // VAO, IBO, VBO
    checksum ^= m->vao ^ m->ibo ^ m->vbo;
    
    // Renderstates
    checksum ^= renderstate_checksum(&m->rs[RENDER_PASS_OPAQUE]);
    checksum ^= renderstate_checksum(&m->rs[RENDER_PASS_TRANSPARENT]);
    checksum ^= renderstate_checksum(&m->rs[RENDER_PASS_CUSTOM]);
    
    // Materials
    for (int i = 0; i < array_count(m->materials); i++) {
        checksum ^= material_checksum(&m->materials[i]);
    }
    
    // Shader uniforms
    checksum ^= model_uniforms_checksum(array_count(m->uniforms), m->uniforms);
    
    return checksum;
}

bool model_bonegetpose(model_t m, unsigned joint, mat34 *out) {
    if(!m.iqm) return false;
    iqm_t *q = m.iqm;

    if(joint >= q->numjoints) return false;

    multiply34x2(*out, q->outframe[joint], q->baseframe[joint]);
    return true;
}

bool model_bonegetposition(model_t m, unsigned joint, mat44 M, vec3 *out) {
    if(!m.iqm) return false;
    iqm_t *q = m.iqm;
    
    mat34 f;
    if (!model_bonegetpose(m, joint, &f)) return false;
    vec3 pos = vec3(f[3],f[7],f[11]);

    pos = transform344(M, pos);
    *out = pos;

    return true;
}

float model_animate_blends(model_t m, anim_t *primary, anim_t *secondary, float delta) {
    if(!m.iqm) return -1;
    iqm_t *q = m.iqm;

    anim_tick(primary, 1, delta);
    anim_tick(secondary, 0, delta);

    float alpha = primary->alpha;
//  if( alpha <= 0 ) return model_animate(m, primary.pose.x);
//  if( alpha >= 1 ) return model_animate(m, secondary.pose.x);

    unsigned frame1 = primary->pose.x;
    unsigned frame2 = primary->pose.y;
    float    alphaA = primary->pose.z;

    unsigned frame3 = secondary->pose.x;
    unsigned frame4 = secondary->pose.y;
    float    alphaB = secondary->pose.z;

    mat34 *mat1 = &q->frames[frame1 * q->numjoints];
    mat34 *mat2 = &q->frames[frame2 * q->numjoints];
    mat34 *mat3 = &q->frames[frame3 * q->numjoints];
    mat34 *mat4 = &q->frames[frame4 * q->numjoints];

    for(int i = 0; i < q->numjoints; i++) {
        mat34 matA, matB, matF;
        lerp34(matA, mat1[i], mat2[i], alphaA);
        lerp34(matB, mat3[i], mat4[i], alphaB);
        lerp34(matF, matA, matB, alpha );
        if(q->joints[i].parent >= 0) multiply34x2(q->outframe[i], q->outframe[q->joints[i].parent], matF);
        else copy34(q->outframe[i], matF);
    }

    return frame1 + alpha;
}

float model_animate_clip(model_t m, float curframe, int minframe, int maxframe, bool loop) {
    if(!m.iqm) return -1;
    iqm_t *q = m.iqm;

    float retframe = -1;
    if( q->numframes > 0 ) {
        vec3 p = pose(curframe >= m.curframe, curframe, minframe, maxframe, loop, &retframe);
        int frame1 = p.x;
        int frame2 = p.y;
        float offset = p.z;

        mat34 *mat1 = &q->frames[frame1 * q->numjoints];
        mat34 *mat2 = &q->frames[frame2 * q->numjoints];

        // @todo: add animation blending and inter-frame blending here
        for(int i = 0; i < q->numjoints; i++) {
            mat34 mat; lerp34(mat, mat1[i], mat2[i], offset);
            if(q->joints[i].parent >= 0) multiply34x2(q->outframe[i], q->outframe[q->joints[i].parent], mat);
            else copy34(q->outframe[i], mat);
        }
    }

    return retframe;
}

void model_skeletonrender(model_t m, mat44 M) {
    if(!m.iqm) return;
    iqm_t *q = m.iqm;

    if(!q->numjoints) return;

    ddraw_ontop_push(true);
    ddraw_color_push(RED);

    for( int joint = 0; joint < q->numjoints; joint++ ) {
        if( q->joints[joint].parent < 0) continue;

        // bone space...
        mat34 f;
        model_bonegetpose(m, joint, &f);
        vec3 pos = vec3(f[3],f[7],f[11]);

        model_bonegetpose(m, q->joints[joint].parent, &f);
        vec3 src = vec3(f[3],f[7],f[11]);

        // ...to model space
        src = transform344(M, src);
        pos = transform344(M, pos);

        // red line
        ddraw_color(RED);
//      ddraw_line(src, pos);
        ddraw_bone(src, pos);

        // green dot
        ddraw_color(GREEN);
        ddraw_point(pos);

        // yellow text
        ddraw_color(YELLOW);
        ddraw_text(pos, 0.005, va("%d", joint));
    }

    ddraw_color_pop();
    ddraw_ontop_pop();
}

float model_animate(model_t m, float curframe) {
    if(!m.iqm) return -1;
    iqm_t *q = m.iqm;
    return model_animate_clip(m, curframe, 0, q->numframes-1, true);
}

static inline
void shader_colormap_model_internal(model_t *m,const char *col_name, const char *bool_name, const char *tex_name, colormap_t c, int slot ) {
    // assumes shader uses `struct { vec4 color; bool has_tex } name + sampler2D name_tex;`
    shader_vec4( col_name, c.color );
    shader_bool( bool_name, c.texture != NULL && (c.texture && c.texture->id != texture_checker().id) );
    if( c.texture ) shader_texture_unit_kind_(GL_TEXTURE_2D, shader_uniform(tex_name), c.texture->id, slot);
    else {
        glUniform1i(shader_uniform(tex_name), slot);
    }
}


typedef struct drawcall_t {
    int mesh;
    union {
        uint64_t order;
        struct {
            uint32_t tex;
            float distance;
        };
    };
} drawcall_t;

static
int drawcall_compare(const void *a, const void *b) {
    const drawcall_t *da = a, *db = b;
    return da->order < db->order ? 1 : da->order > db->order ? -1 : 0;
}

bool model_has_transparency_mesh(model_t m, int mesh) {
    if(!m.iqm) return false;
    iqm_t *q = m.iqm;
    ASSERT(mesh < q->nummeshes);

    if (m.materials[q->mesh_materials[mesh]].layer[MATERIAL_CHANNEL_ALBEDO].map.color.a < 1 || (m.materials[q->mesh_materials[mesh]].layer[MATERIAL_CHANNEL_ALBEDO].map.texture && m.materials[q->mesh_materials[mesh]].layer[MATERIAL_CHANNEL_ALBEDO].map.texture->transparent)) {
        return true;
    }

    return false;
}

bool model_has_transparency(model_t m) {
    if(!m.iqm) return false;
    iqm_t *q = m.iqm;

    for (int i = 0; i < q->nummeshes; i++) {
        if (model_has_transparency_mesh(m, i)) {
            return true;
        }
    }

    return false;
}


void model_frustumset(model_t *m, frustum f) {
    m->frustum_enabled = 1;
    m->frustum_state = f;
}

void model_frustumclear(model_t *m) {
    m->frustum_enabled = 0;
}

static frustum global_frustum;
static mat44 global_frustum_stored_mat_proj;
static mat44 global_frustum_stored_mat_view;

static inline
bool model_is_visible(model_t m, mat44 model_mat, mat44 proj, mat44 view) {
    if(!m.iqm) return false;

    bool is_enabled = m.frustum_enabled;
    frustum fr = m.frustum_state;

#if GLOBAL_FRUSTUM_ENABLED
    if (active_shadowmap) {
        is_enabled = true;
        fr = active_shadowmap->shadow_frustum;
    }

    if (!is_enabled) { /* custom frustum not provided, let's compute one from input call */
        if (memcmp(global_frustum_stored_mat_proj, proj, sizeof(mat44)) != 0 ||
            memcmp(global_frustum_stored_mat_view, view, sizeof(mat44)) != 0) {
            copy44(global_frustum_stored_mat_proj, proj);
            copy44(global_frustum_stored_mat_view, view);
            mat44 proj_modified;
            copy44(proj_modified, proj);
            
            // Increase FOV by GLOBAL_FRUSTUM_FOV_MULTIPLIER
            float fov_scale = 1.0f / GLOBAL_FRUSTUM_FOV_MULTIPLIER;
            proj_modified[0] *= fov_scale;
            proj_modified[5] *= fov_scale;
            mat44 projview; multiply44x2(projview, proj_modified, view);
            global_frustum = frustum_build(projview);
        }
        fr = global_frustum;
        is_enabled = true;
    }
#endif

    if(!is_enabled) return true;

    sphere s = model_bsphere(m, model_mat);

    if (!frustum_test_sphere(fr, s)) {
        return false;
    }

    aabb box = model_aabb(m, model_mat);

    if (!frustum_test_aabb(fr, box)) {
        return false;
    }

    return true;
}

static inline
void model_set_mesh_material(model_t m, int mesh, int shader, int rs_idx) {
    iqm_t *q = m.iqm;
    const material_t *material = &m.materials[q->mesh_materials[mesh]];
    shader_colormap_model_internal(&m, "map_albedo.color", "map_albedo.has_tex", "map_albedo_tex", material->layer[MATERIAL_CHANNEL_ALBEDO].map, MODEL_TEXTURE_ALBEDO);

    shader_float("u_cutout_alpha", material->cutout_alpha);
    shader_float("u_ssr_strength", material->ssr_strength);
    shader_bool("u_parallax_clip", material->parallax_clip);
    shader_bool("u_disable_ibl", material->disable_ibl);

    if (rs_idx != RENDER_PASS_SHADOW) {
        shader_colormap_model_internal(&m, "map_normals.color", "map_normals.has_tex", "map_normals_tex", material->layer[MATERIAL_CHANNEL_NORMALS].map, MODEL_TEXTURE_NORMALS);
        shader_colormap_model_internal(&m, "map_roughness.color", "map_roughness.has_tex", "map_roughness_tex", material->layer[MATERIAL_CHANNEL_ROUGHNESS].map, MODEL_TEXTURE_ROUGHNESS);
        shader_colormap_model_internal(&m, "map_metallic.color", "map_metallic.has_tex", "map_metallic_tex", material->layer[MATERIAL_CHANNEL_METALLIC].map, MODEL_TEXTURE_METALLIC);
        shader_colormap_model_internal(&m, "map_ao.color", "map_ao.has_tex", "map_ao_tex", material->layer[MATERIAL_CHANNEL_AO].map, MODEL_TEXTURE_AO);
        shader_colormap_model_internal(&m, "map_ambient.color", "map_ambient.has_tex", "map_ambient_tex", material->layer[MATERIAL_CHANNEL_AMBIENT].map, MODEL_TEXTURE_AMBIENT);
        shader_colormap_model_internal(&m, "map_emissive.color", "map_emissive.has_tex", "map_emissive_tex", material->layer[MATERIAL_CHANNEL_EMISSIVE].map, MODEL_TEXTURE_EMISSIVE);
        shader_colormap_model_internal(&m, "map_parallax.color", "map_parallax.has_tex", "map_parallax_tex", material->layer[MATERIAL_CHANNEL_PARALLAX].map, MODEL_TEXTURE_PARALLAX);
        shader_float("parallax_scale", material->layer[MATERIAL_CHANNEL_PARALLAX].value);
        shader_float("parallax_shadow_power", material->layer[MATERIAL_CHANNEL_PARALLAX].value2);
        shader_float("u_emissive_value", material->layer[MATERIAL_CHANNEL_EMISSIVE].value);
    }
}

static
void model_draw_call(model_t m, int shader, int pass, vec3 cam_pos, mat44 model_mat, mat44 proj, mat44 view) {
    if(!m.iqm) return;
    iqm_t *q = m.iqm;

    handle old_shader = last_shader;
    shader_bind(shader);

    int rs_idx = pass == -1 ? RENDER_PASS_OPAQUE : pass;
    renderstate_t *rs = &m.rs[rs_idx];
    renderstate_apply(rs);

    glBindVertexArray( q->vao );

    static array(int) required_rs = 0;
    array_resize(required_rs, q->nummeshes);

    for(int i = 0; i < q->nummeshes; i++) {
        struct iqmmesh *im = &q->meshes[i];
        required_rs[i] = rs_idx;

        if (required_rs[i] < RENDER_PASS_OVERRIDES_BEGIN) {
            if (model_has_transparency_mesh(m, i)) {
                required_rs[i] = RENDER_PASS_TRANSPARENT;
            }
        }
    }

    static array(drawcall_t) drawcalls = 0;
    array_resize(drawcalls, 0);

    if (rs_idx > RENDER_PASS_OVERRIDES_BEGIN) {
        for(int i = 0; i < q->nummeshes; i++) {
            array_push(drawcalls, (drawcall_t){i, -1});
        }
    } else {
        if(pass == -1 || pass == RENDER_PASS_OPAQUE) {
            for(int i = 0; i < q->nummeshes; i++) {
                // collect opaque drawcalls
                if (required_rs[i] == RENDER_PASS_OPAQUE) {
                    drawcall_t call;
                    call.mesh = i;
                    call.distance = -1;
                    call.tex = m.materials[q->mesh_materials[i]].layer[MATERIAL_CHANNEL_ALBEDO].map.texture ? m.materials[q->mesh_materials[i]].layer[MATERIAL_CHANNEL_ALBEDO].map.texture->id : m.materials[q->mesh_materials[i]].layer[MATERIAL_CHANNEL_ALBEDO].map.texture ? m.materials[q->mesh_materials[i]].layer[MATERIAL_CHANNEL_ALBEDO].map.texture->id : texture_checker().id;
                    array_push(drawcalls, call);
                }
            }
        }
        
        if(pass == -1 || pass == RENDER_PASS_TRANSPARENT) {
            for(int i = 0; i < q->nummeshes; i++) {
                // collect transparent drawcalls
                if (required_rs[i] == RENDER_PASS_TRANSPARENT) {
                    drawcall_t call;
                    call.mesh = i;
                    call.tex = m.materials[q->mesh_materials[i]].layer[MATERIAL_CHANNEL_ALBEDO].map.texture ? m.materials[q->mesh_materials[i]].layer[MATERIAL_CHANNEL_ALBEDO].map.texture->id : m.materials[q->mesh_materials[i]].layer[MATERIAL_CHANNEL_ALBEDO].map.texture ? m.materials[q->mesh_materials[i]].layer[MATERIAL_CHANNEL_ALBEDO].map.texture->id : texture_checker().id;
                    struct iqmmesh *im = &q->meshes[i];
                    
                    // calculate distance from camera
                    {
                        vec3 pos = ptr3(((struct iqm_vertex*)m.verts)[im->first_vertex].position);
                        call.distance = len3sq(sub3(cam_pos, transform344(model_mat, pos)));
                    }
                    array_push(drawcalls, call);
                }
            }
        }
    }

    // sort drawcalls by order
    array_sort(drawcalls, drawcall_compare);

    struct iqmtriangle *tris = NULL;
    for(int di = 0; di < array_count(drawcalls); di++) {
        int i = drawcalls[di].mesh;
        struct iqmmesh *im = &q->meshes[i];

        if (pass != -1 && pass != required_rs[i]) continue;
        
        if (rs_idx != required_rs[i]) {
            rs_idx = required_rs[i];
            rs = &m.rs[rs_idx];
            renderstate_apply(rs);
        }
        
        model_set_mesh_material(m, i, shader, rs_idx);

        glDrawElementsInstanced(GL_TRIANGLES, 3*im->num_triangles, GL_UNSIGNED_INT, &tris[im->first_triangle], m.num_instances);
        profile_incstat("Render.num_drawcalls", +1);
        profile_incstat("Render.num_triangles", +im->num_triangles*m.num_instances);
    }

    glBindVertexArray( 0 );

    shader_bind(old_shader);
}

static mat44 *pass_model_matrices = NULL;

void model_render(model_t *mdl, mat44 proj, mat44 view, mat44* models, unsigned count, int pass) {
    if(!mdl->iqm) return;
    iqm_t *q = mdl->iqm;

    if (q->vbo == 0 || q->ibo == 0) {
        return;
    }

    if (active_shadowmap && active_shadowmap->skip_render) {
        return;
    }

    bool has_shadows = false;
    if (mdl->lights.count > 0) {
        model_addswitch(mdl, "USE_LIGHTING");
        for (int i = 0; i < mdl->lights.count; i++) {
            if (mdl->lights.base[i].cast_shadows) {
                has_shadows = true;
                break;
            }
        }
        if (has_shadows) {
            model_addswitch(mdl, "USE_SHADOWS");
        }
    } else {
        model_delswitch(mdl, "USE_LIGHTING");
    }

    if (!has_shadows) {
        model_delswitch(mdl, "USE_SHADOWS");
    }

    pass_model_matrices = array_resize(pass_model_matrices, count); //@leak
    memcpy(pass_model_matrices, models, count * sizeof(mat44));

    for (int i = 0; i < count; i++) {
        bool visible = model_is_visible(*mdl, pass_model_matrices[i], proj, view);
        if (!visible) {
            array_erase_fast(pass_model_matrices, i);
            i--;
            count--;
        }
    }

    if (count == 0) {
        return;
    }

    mdl->iqm->texture_unit = 0;

    mat44 mv; multiply44x2(mv, view, pass_model_matrices[0]);

    if( count != mdl->num_instances ) {
        mdl->num_instances = count;
        mdl->instanced_matrices = (float*)pass_model_matrices;
        model_set_state(*mdl);
    }

    int shader = mdl->iqm->program;
    if (pass == RENDER_PASS_SHADOW) {
        shader = mdl->iqm->shadow_program;
        model_set_uniforms(*mdl, shader, mv, proj, view, pass_model_matrices[0]);
        model_draw_call(*mdl, shader, pass, pos44(view), pass_model_matrices[0], proj, view);
        return;
    }

    if (pass == RENDER_PASS_MATERIAL) {
        shader = mdl->iqm->material_program;
        model_set_uniforms(*mdl, shader, mv, proj, view, pass_model_matrices[0]);
        model_draw_call(*mdl, shader, pass, pos44(view), pass_model_matrices[0], proj, view);
        return;
    }

    shader_bind(shader);
    light_update(&q->light_ubo, mdl->lights.count, mdl->lights.base);

    model_set_uniforms(*mdl, shader, mv, proj, view, pass_model_matrices[0]);
    model_draw_call(*mdl, shader, pass, pos44(view), pass_model_matrices[0], proj, view);
    
    if (q->light_ubo) {
        ubo_unbind(q->light_ubo);
    }
}

void model_setshader(model_t *m, const char *vs, const char *fs, const char *defines) {
    if (!m->iqm) return;
    iqm_t *q = m->iqm;

    int flags = m->stored_flags;
    
    for (int i = 0; i < array_count(m->materials); ++i) {
        model_load_pbr(m, &m->materials[i]);
    }

    if (!vs) {
        vs = file_read("shaders/model_vs.gl",0);
    }

    if (!fs) {
        fs = file_read("shaders/model_fs.gl",0);
    }


    const char *switches = array_count(m->shaderinfo.switches) ? strjoin(m->shaderinfo.switches, ",") : NULL;
    const char *final_defines = defines;

    if (!defines) {
        final_defines = "NO_CUSTOM_DEFINES";
    }
    if (switches && switches[0]) {
        final_defines = (const char *)va("%s,%s", final_defines, switches);
    }
    

    // rebind shader
    glUseProgram(0);
    if (q->program) {
        glDeleteProgram(q->program);
    }
    if (q->shadow_program) {
        glDeleteProgram(q->shadow_program);
    }

    {
        int shaderprog = shader(vs, fs,
            "att_position,att_texcoord,att_normal,att_tangent,att_instanced_matrix,,,,att_indexes,att_weights,att_vertexindex,att_color,att_bitangent,att_texcoord2","fragColor",
            final_defines);
        q->program = shaderprog;
    }

    {
        int shaderprog = shader(vs, file_read("shaders/shadow_pass_fs.gl",0),
            "att_position,att_texcoord,att_normal,att_tangent,att_instanced_matrix,,,,att_indexes,att_weights,att_vertexindex,att_color,att_bitangent,att_texcoord2","fragcolor",
            va("SHADOW_CAST,%s", final_defines));
        q->shadow_program = shaderprog;
    }
    
    {
        int shaderprog = shader(vs, file_read("shaders/model_material_fs.gl",0),
            "att_position,att_texcoord,att_normal,att_tangent,att_instanced_matrix,,,,att_indexes,att_weights,att_vertexindex,att_color,att_bitangent,att_texcoord2",NULL,
            final_defines);
        q->material_program = shaderprog;

        glBindFragDataLocation(shaderprog, 0, "out_matprops");
        glBindFragDataLocation(shaderprog, 1, "out_normals");
        glBindFragDataLocation(shaderprog, 2, "out_albedo");
    }
    model_init_uniforms(q, 0, q->program);
    model_init_uniforms(q, 1, q->shadow_program);
    model_init_uniforms(q, 2, q->material_program);

    unsigned block_index = glGetUniformBlockIndex(q->program, "LightBuffer");
    glUniformBlockBinding(q->program, block_index, 0);

    if (m->shaderinfo.vs) {
        FREE(m->shaderinfo.vs);
    }
    if (m->shaderinfo.fs) {
        FREE(m->shaderinfo.fs);
    }
    if (m->shaderinfo.defines) {
        FREE(m->shaderinfo.defines);
    }

    m->shaderinfo.vs = vs ? STRDUP(vs) : NULL;
    m->shaderinfo.fs = fs ? STRDUP(fs) : NULL;
    m->shaderinfo.defines = defines ? STRDUP(defines) : NULL;
}

void model_skybox(model_t *mdl, skybox_t sky) {
    model_cubemap(mdl, &sky.cubemap);
    mdl->sky_cubemap.id = sky.cubemap.id;
    mdl->sky_refl = sky.refl;
    mdl->sky_env = sky.env;
}

void model_cubemap(model_t *mdl, cubemap_t *c) {
    static char coef_names[9][128] = {0};
    do_once {
        for (int i = 0; i < 9; i++) {
            sprintf(coef_names[i], "u_coefficients_sh[%d]", i);
        }
    }
    for (int i = 0; i < 9; i++) {
        model_adduniform(mdl, model_uniform(coef_names[i], UNIFORM_VEC3, .v3 = c ? c->sh[i] : vec3(0,0,0)));
    }
}

void model_probe(model_t *mdl, vec3 center, float radius, unsigned count, cubemap_t *c) {
    vec3 sh[9] = {0};
    cubemap_sh_blend(center, radius, count, c, sh);
    cubemap_t sh_cubemap = {0};
    memcpy(sh_cubemap.sh, sh, sizeof(sh));
    model_cubemap(mdl, &sh_cubemap);
}

void model_shadow(model_t *mdl, shadowmap_t *sm) {
    if (sm) {
        mdl->shadow_receiver = true;
        mdl->shadow_map = sm;
    } else {
        mdl->shadow_receiver = false;
        mdl->shadow_map = NULL;
    }
}

void model_light(model_t *mdl, unsigned count, light_t *lights) {
    mdl->lights.base = lights;
    mdl->lights.count = count;
}

void model_fog(model_t *mdl, unsigned mode, vec3 color, float start, float end, float density) {
    model_adduniform(mdl, model_uniform("u_fog_color", UNIFORM_VEC3, .v3 = color));
    model_adduniform(mdl, model_uniform("u_fog_density", UNIFORM_FLOAT, .f = density));
    model_adduniform(mdl, model_uniform("u_fog_start", UNIFORM_FLOAT, .f = start));
    model_adduniform(mdl, model_uniform("u_fog_end", UNIFORM_FLOAT, .f = end));
    model_adduniform(mdl, model_uniform("u_fog_type", UNIFORM_INT, .i = mode));
}

void model_applyuniform(model_t m, const model_uniform_t *t) {
    if (!m.iqm) return;
    iqm_t *q = m.iqm;

    int oldprog = last_shader;
    shader_bind(q->program);

    switch (t->kind) {
    case UNIFORM_FLOAT:
        glUniform1f(shader_uniform(t->name), t->f);
        break;
    case UNIFORM_INT:
    case UNIFORM_BOOL:
        glUniform1i(shader_uniform(t->name), t->i);
    case UNIFORM_UINT:
        glUniform1ui(shader_uniform(t->name), t->u);
        break;
    case UNIFORM_VEC2:
        glUniform2fv(shader_uniform(t->name), 1, &t->v2.x);
        break;
    case UNIFORM_VEC3:
        glUniform3fv(shader_uniform(t->name), 1, &t->v3.x);
        break;
    case UNIFORM_VEC4:
        glUniform4fv(shader_uniform(t->name), 1, &t->v4.x);
        break;
    case UNIFORM_MAT3:
        glUniformMatrix3fv(shader_uniform(t->name), 1, GL_FALSE, t->m33);
        break;
    case UNIFORM_MAT4:
        glUniformMatrix4fv(shader_uniform(t->name), 1, GL_FALSE, t->m44);
        break;
    case UNIFORM_SAMPLER2D:
        shader_texture_unit_kind_(GL_TEXTURE_2D, shader_uniform(t->name), t->u, model_texture_unit(&m));
        break;
    case UNIFORM_SAMPLER3D:
        shader_texture_unit_kind_(GL_TEXTURE_3D, shader_uniform(t->name), t->u, model_texture_unit(&m));
        break;
    case UNIFORM_SAMPLERCUBE:
        shader_texture_unit_kind_(GL_TEXTURE_CUBE_MAP, shader_uniform(t->name), t->u, model_texture_unit(&m));
        break;
    }

    shader_bind(oldprog);
}

void model_adduniform(model_t* m, model_uniform_t uniform) {
    for (unsigned i = 0; i < array_count(m->uniforms); i++) {
        if (strcmp(m->uniforms[i].name, uniform.name) == 0) {
            m->uniforms[i] = uniform;
            return;
        }
    }
    array_push(m->uniforms, uniform);
}

void model_adduniforms(model_t* m, unsigned count, model_uniform_t* uniforms) {
    for (unsigned i = 0; i < count; i++) {
        model_adduniform(m, uniforms[i]);
    }
}

static inline
void model_rebuild_shader(model_t *m) {
    model_setshader(m, STRDUP(m->shaderinfo.vs), STRDUP(m->shaderinfo.fs), m->shaderinfo.defines ? STRDUP(m->shaderinfo.defines) : NULL);
}

void model_addswitch(model_t *m, const char *name) {
    for (unsigned i = 0; i < array_count(m->shaderinfo.switches); i++) {
        if (strcmp(m->shaderinfo.switches[i], name) == 0) {
            return;
        }
    }
    array_push(m->shaderinfo.switches, STRDUP(name));
    model_rebuild_shader(m);
}

void model_delswitch(model_t *m, const char *name) {
    for (unsigned i = 0; i < array_count(m->shaderinfo.switches); i++) {
        if (strcmp(m->shaderinfo.switches[i], name) == 0) {
            FREE(m->shaderinfo.switches[i]);
            array_erase_fast(m->shaderinfo.switches, i);
            model_rebuild_shader(m);
            return;
        }
    }
}

// static
aabb aabb_transform( aabb A, mat44 M ) {
    // Based on "Transforming Axis-Aligned Bounding Boxes" by Jim Arvo, 1990
    aabb B = { {M[12],M[13],M[14]}, {M[12],M[13],M[14]} }; // extract translation from mat44
    for( int i = 0; i < 3; i++ )
    for( int j = 0; j < 3; j++ ) {
        float a = M[i*4+j] * j[&A.min.x]; // use mat33 from mat44
        float b = M[i*4+j] * j[&A.max.x]; // use mat33 from mat44
        if( a < b ) {
            i[&B.min.x] += a;
            i[&B.max.x] += b;
        } else {
            i[&B.min.x] += b;
            i[&B.max.x] += a;
        }
    }
    return B;
}

aabb model_aabb(model_t m, mat44 transform) {
    iqm_t *q = m.iqm;
    if( q && q->bounds ) {
    int f = ( (int)m.curframe ) % (q->numframes + !q->numframes);
    vec3 bbmin = ptr3(q->bounds[f].bbmin);
    vec3 bbmax = ptr3(q->bounds[f].bbmax);
    return aabb_transform(aabb(bbmin,bbmax), transform);
    }
    return aabb(vec3(0,0,0),vec3(0,0,0));
}

sphere model_bsphere(model_t m, mat44 transform) {
    iqm_t *q = m.iqm;
    if( q && q->bounds ) {
    int f = ( (int)m.curframe ) % (q->numframes + !q->numframes);
    vec3 bbmin = ptr3(q->bounds[f].bbmin);
    vec3 bbmax = ptr3(q->bounds[f].bbmax);
    aabb box = aabb_transform(aabb(bbmin,bbmax), transform);
    return sphere(scale3(add3(box.min, box.max), 0.5f), 0.5f * (len3(sub3(box.min, box.max))));
    }
    return sphere(vec3(0,0,0), 0);
}

void model_destroy(model_t m) {
    bool can_delete = m.flags & MODEL_UNIQUE;

    if (can_delete) {
        for (int i = 0; i < array_count(model_cache); i++) {
            if (model_cache[i].filename == m.filename) {
                array_erase_slow(model_cache, i);
                break;
            }
        }
        FREE(m.verts);
        for( int i = 0, end = array_count(m.texture_names); i < end; ++i ) {
            FREE(m.texture_names[i]);
        }
        array_free(m.texture_names);
        array_free(m.materials);

        if (m.iqm->light_ubo) {
            ubo_destroy(m.iqm->light_ubo);
        }

        iqm_t *q = m.iqm;
        FREE(q->outframe);
        FREE(q->mesh_materials);
        FREE(q->baseframe);
        FREE(q->inversebaseframe);
        if(q->animdata != q->meshdata) FREE(q->animdata);
        FREE(q->frames);
        FREE(q->buf);
        FREE(q);
        array_free(m.uniforms);
    }
}

#endif
