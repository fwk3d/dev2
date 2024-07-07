// -----------------------------------------------------------------------------
// lights

#if !CODE

enum LIGHT_TYPE {
    LIGHT_DIRECTIONAL,
    LIGHT_POINT,
    LIGHT_SPOT,
};

enum SHADOW_TECHNIQUE {
    SHADOW_VSM,
    SHADOW_CSM,
};

typedef struct light_t { OBJ
    char *name;
    unsigned type;
    vec3 diffuse, specular, ambient;
    vec3 pos, dir;
    struct {
        float constant, linear, quadratic;
    } falloff;
    float radius;
    float specularPower;
    float innerCone, outerCone;
    //@todo: cookie, flare

    // Shadowmapping
    bool cast_shadows;
    bool hard_shadows;
    unsigned shadow_technique;
    float shadow_distance;
    float shadow_near_clip;
    mat44 shadow_matrix[NUM_SHADOW_CASCADES];
    float min_variance; //< VSM
    float variance_transition; //< VSM
    float shadow_bias; //< CSM
    float normal_bias; //< CSM
    float shadow_softness;
    float penumbra_size;

    // internals
    bool cached; //< used by scene to invalidate cached light data
    bool processed_shadows;
} light_t;
OBJTYPEDEF(light_t,OBJTYPE_light);

API light_t light();
// API void    light_flags(int flags);
API void    light_type(light_t* l, char type);
API void    light_diffuse(light_t* l, vec3 color);
API void    light_specular(light_t* l, vec3 color);
API void    light_ambient(light_t* l, vec3 color);
API void    light_pos(light_t* l, vec3 pos);
API void    light_dir(light_t* l, vec3 dir);
API void    light_power(light_t* l, float power);
API void    light_radius(light_t* l, float radius);
API void    light_falloff(light_t* l, float constant, float linear, float quadratic);
API void    light_cone(light_t* l, float innerCone, float outerCone);

API void    ui_light(light_t *l);
API void    ui_lights(unsigned num_lights, light_t *lights);

#else

// -----------------------------------------------------------------------------

static
void light_ctor(light_t *l) {
    l->diffuse = vec3(1,1,1);
    l->dir = vec3(1,-1,-1);
    l->falloff.constant = 1.0f;
    l->falloff.linear = 0.09f;
    l->falloff.quadratic = 0.0032f;
    l->specularPower = 32.f;
    l->innerCone = 0.85f;// 31 deg
    l->outerCone = 0.9f; // 25 deg
    l->cast_shadows = true;
    l->processed_shadows = false;
    l->hard_shadows = false;
    l->shadow_distance = 400.0f;
    l->shadow_near_clip = 0.01f;
    l->shadow_bias = 0.001f;
    l->normal_bias = 0.003f;
    l->shadow_softness = 1.5f;
    l->penumbra_size = 2.0f;
    l->min_variance = 0.00002f;
    l->variance_transition = 0.2f;
}

#if HAS_OBJ
AUTORUN {
    STRUCT(light_t, char*, name, "Light name");
    STRUCT(light_t, unsigned, type, "Light type");
    STRUCT(light_t, vec3, diffuse, "Diffuse color");
    STRUCT(light_t, vec3, specular, "Specular color");
    STRUCT(light_t, vec3, ambient, "Ambient color");
    STRUCT(light_t, vec3, pos, "Position");
    STRUCT(light_t, vec3, dir, "Direction");
    STRUCT(light_t, float, falloff.constant, "Falloff constant");
    STRUCT(light_t, float, falloff.linear, "Falloff linear");
    STRUCT(light_t, float, falloff.quadratic, "Falloff quadratic");
    STRUCT(light_t, float, radius, "Radius");
    STRUCT(light_t, float, specularPower, "Specular power");
    STRUCT(light_t, float, innerCone, "Inner cone angle");
    STRUCT(light_t, float, outerCone, "Outer cone angle");
    //@todo: cookie, flare

    // Shadowmapping
    STRUCT(light_t, bool, cast_shadows, "Cast shadows flag");
    STRUCT(light_t, bool, hard_shadows, "Hard shadows flag");
    STRUCT(light_t, unsigned, shadow_technique, "Shadow technique");
    STRUCT(light_t, float, shadow_distance, "Shadow distance");
    STRUCT(light_t, float, shadow_near_clip, "Shadow near clip distance");
    STRUCT(light_t, mat44[NUM_SHADOW_CASCADES], shadow_matrix, "Shadow matrices");
    STRUCT(light_t, float, min_variance, "Minimum variance for VSM");
    STRUCT(light_t, float, variance_transition, "Variance transition for VSM");
    STRUCT(light_t, float, shadow_bias, "Shadow bias for CSM");
    STRUCT(light_t, float, normal_bias, "Normal bias for CSM");
    STRUCT(light_t, float, shadow_softness, "Shadow softness");
    STRUCT(light_t, float, penumbra_size, "Penumbra size");

    // internals
    STRUCT(light_t, bool, cached, "_Cached flag");
    STRUCT(light_t, bool, processed_shadows, "_Processed shadows flag");

    obj_ctor[OBJTYPE_light] = light_ctor;
}
#endif

light_t light() {
    light_t l = {0};
    light_ctor(&l);
    return l;
}

void light_type(light_t* l, char type) {
    l->cached = 0;
    l->type = type;
}

void light_diffuse(light_t* l, vec3 color) {
    l->cached = 0;
    l->diffuse = color;
}

void light_specular(light_t* l, vec3 color) {
    l->cached = 0;
    l->specular = color;
}

void light_ambient(light_t* l, vec3 color) {
    l->cached = 0;
    l->ambient = color;
}

void light_pos(light_t* l, vec3 pos) {
    l->cached = 0;
    l->pos = pos;
}

void light_dir(light_t* l, vec3 dir) {
    l->cached = 0;
    l->dir = dir;
}

void light_power(light_t* l, float power) {
    l->cached = 0;
    l->specularPower = power;
}

void light_falloff(light_t* l, float constant, float linear, float quadratic) {
    l->cached = 0;
    l->falloff.constant = constant;
    l->falloff.linear = linear;
    l->falloff.quadratic = quadratic;
}

void light_radius(light_t* l, float radius) {
    l->cached = 0;
    l->radius = radius;
}

void light_cone(light_t* l, float innerCone, float outerCone) {
    l->cached = 0;
    l->innerCone = acos(innerCone);
    l->outerCone = acos(outerCone);
}

array(light_t) lightlist(const char *pathfile) {
    light_t *lightlist = 0;
    char *light_file = file_read(strendi(pathfile,".txt") ? pathfile : va("%s@lightlist.txt", pathfile), 0);
    if( light_file ) {
        // deserialize light
        for each_substring(light_file, "\r\n", light_line) {
            light_t l = light();
            int type;
            vec3 position, direction, color;
            float power;
            char name[128] = {0};

            if (sscanf(light_line, "light: %d %f %f %f %f %f %f %f %f %f %f %127[^\n]",
                       &type, &position.x, &position.y, &position.z,
                       &direction.x, &direction.y, &direction.z,
                       &color.x, &color.y, &color.z,
                       &power, name) != 12) continue;

            l.type = type;
            l.pos = position;
            l.dir = direction;
            // l.diffuse = scale3(color, power);
            l.diffuse = color;
            l.radius = power;
            float falloff = power*0.005f;
            // recalculate attenuation based on radius
            l.falloff.constant = 1.0f/power;
            l.falloff.linear = 0.0f;
            l.falloff.quadratic = 1.0f/falloff/power;
            l.name = STRDUP(name); //@leak

            array_push(lightlist, l);
        }
    }
    return lightlist;
}

static inline
char *light_fieldname(const char *fmt, ...) {
    static char buf[32];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    return buf;
}

typedef struct light_node_t {
    vec4 diffuse;
    vec4 specular;
    vec4 ambient;
    vec4 pos;
    vec4 dir;
    float power;
    float radius;
    float innerCone;
    float outerCone;
    float constant;
    float linear;
    float quadratic;
    float shadow_bias;
    float normal_bias;
    float min_variance;
    float variance_transition;
    float shadow_softness;
    float penumbra_size;
    int type;
    int processed_shadows;
    int hard_shadows;
} light_node_t;

static inline
void light_update(unsigned* ubo, unsigned num_lights, light_t *lv) {
    if (num_lights > MAX_LIGHTS) {
        num_lights = MAX_LIGHTS;
    }

    light_node_t lights[MAX_LIGHTS] = {0};
    for (unsigned i = 0; i < num_lights; i++) {
        light_node_t *light = &lights[i];
        {
            light->type = lv[i].type;
            light->diffuse = vec34(lv[i].diffuse, 0.0f);
            light->specular = vec34(lv[i].specular, 0.0f);
            light->ambient = vec34(lv[i].ambient, 0.0f);
            light->pos = vec34(lv[i].pos, 0.0f);
            light->dir = vec34(lv[i].dir, 0.0f);
            light->power = lv[i].specularPower;
            light->radius = lv[i].radius;
            light->innerCone = lv[i].innerCone;
            light->outerCone = lv[i].outerCone;
            light->processed_shadows = !lv[i].cast_shadows ? false : lv[i].processed_shadows;
            light->hard_shadows = lv[i].hard_shadows;
            light->constant = lv[i].falloff.constant;
            light->linear = lv[i].falloff.linear;
            light->quadratic = lv[i].falloff.quadratic;
            light->shadow_bias = lv[i].shadow_bias;
            light->normal_bias = lv[i].normal_bias;
            light->min_variance = lv[i].min_variance;
            light->variance_transition = lv[i].variance_transition;
            light->shadow_softness = lv[i].shadow_softness;
            light->penumbra_size = lv[i].penumbra_size;
        }
    }

    shader_int("u_num_lights", num_lights);
    ASSERT(ubo);

    if (num_lights == 0) {
        return;
    }

    if (*ubo == 0 /* buffer not created */) {
        *ubo = ubo_create(&lights[0], sizeof(light_node_t) * MAX_LIGHTS, STREAM_DRAW);
    } else {
        ubo_update(*ubo, 0, &lights[0], sizeof(light_node_t) * num_lights);
    }

    ubo_bind(*ubo, 0);

    for (unsigned i=0; i < num_lights; ++i) {
        lv[i].cached = 1;
        bool processed = false;
        if (lv[i].processed_shadows && lv[i].shadow_technique == SHADOW_CSM) {
            processed = true;
            for (int j = 0; j < NUM_SHADOW_CASCADES; j++) {
                shader_mat44(light_fieldname("light_shadow_matrix_csm[%d]", j), lv[i].shadow_matrix[j]);
            }
        }
        if (processed) break;
    }
}

void ui_light(light_t *l) {
    ui_int("Type", &l->type);
    ui_float3("Position", &l->pos.x);
    ui_float3("Direction", &l->dir.x);
    ui_color3f("Diffuse", &l->diffuse.x);
    ui_color3f("Specular", &l->specular.x);
    ui_color3f("Ambient", &l->ambient.x);
    ui_float("Specular Power", &l->specularPower);
    ui_floatabs("Radius", &l->radius);
    ui_floatabs("Constant Falloff", &l->falloff.constant);
    ui_floatabs("Linear Falloff", &l->falloff.linear);
    ui_floatabs("Quadratic Falloff", &l->falloff.quadratic);
    ui_float("Inner Cone", &l->innerCone);
    ui_float("Outer Cone", &l->outerCone);
    ui_bool("Cast Shadows", &l->cast_shadows);
    ui_bool("Hard Shadows", &l->hard_shadows);
    ui_floatabs("Shadow Distance", &l->shadow_distance);
    ui_floatabs("Shadow Bias", &l->shadow_bias);
    ui_floatabs("Normal Bias", &l->normal_bias);
    ui_floatabs("Shadow Softness", &l->shadow_softness);
    ui_floatabs("Penumbra Size", &l->penumbra_size);
    ui_floatabs("Min Variance", &l->min_variance);
    ui_floatabs("Variance Transition", &l->variance_transition);
}

void ui_lights(unsigned num_lights, light_t *lights) {
    for (unsigned i = 0; i < num_lights; ++i) {
        if (ui_collapse(va("Light %d", i), va("light_%d", i))) {
            ui_light(&lights[i]);
            ui_collapse_end();
        }
    }
}

#endif
