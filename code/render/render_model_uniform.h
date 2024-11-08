// -----------------------------------------------------------------------------
// model uniforms

#if !CODE

enum UNIFORM_KIND {
    UNIFORM_BOOL,
    UNIFORM_INT,
    UNIFORM_UINT,
    UNIFORM_FLOAT,
    UNIFORM_VEC2,
    UNIFORM_VEC3,
    UNIFORM_VEC4,
    UNIFORM_MAT3,
    UNIFORM_MAT4,
    UNIFORM_SAMPLER2D,
    UNIFORM_SAMPLER3D,
    UNIFORM_SAMPLERCUBE,
};

enum MODEL_UNIFORMS {
    MODEL_UNIFORM_MV,
    MODEL_UNIFORM_MVP,
    MODEL_UNIFORM_VP,
    MODEL_UNIFORM_CAM_POS,
    MODEL_UNIFORM_CAM_DIR,
    MODEL_UNIFORM_BILLBOARD,
    MODEL_UNIFORM_MODEL,
    MODEL_UNIFORM_VIEW,
    MODEL_UNIFORM_INV_VIEW,
    MODEL_UNIFORM_PROJ,
    MODEL_UNIFORM_SKINNED,
    MODEL_UNIFORM_INSTANCED,
    MODEL_UNIFORM_VS_BONE_MATRIX,
    MODEL_UNIFORM_U_MATCAPS,
    MODEL_UNIFORM_FRAME_COUNT,
    MODEL_UNIFORM_FRAME_TIME,
    MODEL_UNIFORM_SHADOW_CAMERA_TO_SHADOW_VIEW,
    MODEL_UNIFORM_SHADOW_CAMERA_TO_SHADOW_PROJECTOR,
    MODEL_UNIFORM_SHADOW_TECHNIQUE,
    MODEL_UNIFORM_U_SHADOW_RECEIVER,
    MODEL_UNIFORM_U_BLEND_REGION,
    MODEL_UNIFORM_SHADOW_OFFSETS,
    MODEL_UNIFORM_SHADOW_FILTER_SIZE,
    MODEL_UNIFORM_SHADOW_WINDOW_SIZE,

    // PBR
    MODEL_UNIFORM_RESOLUTION,
    MODEL_UNIFORM_HAS_TEX_ENV_CUBEMAP,
    MODEL_UNIFORM_HAS_TEX_SKYSPHERE,
    MODEL_UNIFORM_HAS_TEX_SKYENV,
    MODEL_UNIFORM_TEX_ENV_CUBEMAP,
    MODEL_UNIFORM_TEX_SKYSPHERE,
    MODEL_UNIFORM_SKYSPHERE_MIP_COUNT,
    MODEL_UNIFORM_TEX_SKYENV,
    MODEL_UNIFORM_TEX_BRDF_LUT,
    
    // Shadows
    MODEL_UNIFORM_MAX_SHADOW_CASCADES,
    MODEL_UNIFORM_SHADOW_CASCADE_DISTANCES,
    MODEL_UNIFORM_SHADOW_CASCADE_DISTANCES_COUNT = MODEL_UNIFORM_SHADOW_CASCADE_DISTANCES+NUM_SHADOW_CASCADES,

    MODEL_UNIFORM_SHADOW_MAP_2D,
    MODEL_UNIFORM_SHADOW_MAP_2D_COUNT = MODEL_UNIFORM_SHADOW_MAP_2D+NUM_SHADOW_CASCADES,
    
    MODEL_UNIFORM_SHADOW_MAP_CUBEMAP,
    MODEL_UNIFORM_SHADOW_MAP_CUBEMAP_COUNT = MODEL_UNIFORM_SHADOW_MAP_CUBEMAP+MAX_LIGHTS,

    NUM_MODEL_UNIFORMS
};

typedef struct model_uniform_t {
    const char *name;
    int kind;
    union {
        float f;
        int i;
        bool b;
        unsigned u;
        vec2 v2;
        vec3 v3;
        vec4 v4;
        mat33 m33;
        mat44 m44;
    };
} model_uniform_t;

#define model_uniform(name, kind, ...) (model_uniform_t){ name, kind, __VA_ARGS__ }

API bool model_compareuniform(const model_uniform_t *a, const model_uniform_t *b);
API bool model_compareuniforms(unsigned s1, const model_uniform_t *a, unsigned s2, const model_uniform_t *b);
API uint32_t model_uniforms_checksum(unsigned count, model_uniform_t *uniforms);

#else

#endif
