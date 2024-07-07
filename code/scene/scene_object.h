// -----------------------------------------------------------------------------
// object

#if !CODE

typedef struct object_t {
    uint64_t renderbucket;
    mat44 transform;
    quat rot;
    vec3 sca, pos, euler, pivot;
    array(texture_t) textures;
    model_t model, model_shadow;
    anim_t anim;
    float anim_speed;
    aabb bounds;
    unsigned billboard; // [0..7] x(4),y(2),z(1) masks
    bool disable_frustum_check;
    bool cast_shadows;
    bool fullbright;
    bool batchable;

    // internal states
    array(handle) old_texture_ids;
    array(texture_t) old_textures;
    float distance;
    bool skip_draw;
    bool light_cached; //< used by scene to update light data
    bool was_batched;
    array(mat44) instances;
    unsigned num_instances;
    array(unsigned) pair_instance;
    uint32_t checksum;
} object_t;

API object_t object();
API bool object_compare(object_t *obj1, object_t *obj2);
API void object_rotate(object_t *obj, vec3 euler);
API void object_pivot(object_t *obj, vec3 euler);
API void object_teleport(object_t *obj, vec3 pos);
API void object_move(object_t *obj, vec3 inc);
API vec3 object_position(object_t *obj);
API void object_scale(object_t *obj, vec3 sca);
API void object_batchable(object_t *obj, bool batchable);
//
API void object_model(object_t *obj, model_t model);
API void object_model_shadow(object_t *obj, model_t model);
API void object_anim(object_t *obj, anim_t anim, float speed);
API void object_diffuse(object_t *obj, texture_t tex);
API void object_diffuse_push(object_t *obj, texture_t tex);
API void object_diffuse_pop(object_t *obj);
API void object_billboard(object_t *obj, unsigned mode);

#else

static
void object_update(object_t *obj) {
    quat p = eulerq(vec3(obj->pivot.x,obj->pivot.y,obj->pivot.z));
    quat e = eulerq(vec3(obj->euler.x,obj->euler.y,obj->euler.z));
    compose44(obj->transform, obj->pos, mulq(e, p), obj->sca);
}

object_t object() {
    object_t obj = {0};
    identity44(obj.transform);
    //obj.rot = idq();
    obj.sca = vec3(1,1,1);
    //obj.bounds = aabb(vec3(0,0,0),vec3(1,1,1)); // defaults to small 1-unit cube
    object_rotate(&obj, vec3(0,0,0));
    //array_init(obj.textures);
    obj.cast_shadows = true;
    obj.batchable = true;
    return obj;
}

bool object_compare(object_t *obj1, object_t *obj2) {
    // if (obj1->renderbucket != obj2->renderbucket) return false;
    // if (memcmp(obj1->transform, obj2->transform, sizeof(mat44))) return false;
    // if (memcmp(&obj1->rot, &obj2->rot, sizeof(quat))) return false;
    // if (memcmp(&obj1->sca, &obj2->sca, sizeof(vec3))) return false;
    // if (memcmp(&obj1->pos, &obj2->pos, sizeof(vec3))) return false;
    // if (memcmp(&obj1->euler, &obj2->euler, sizeof(vec3))) return false;
    // if (memcmp(&obj1->pivot, &obj2->pivot, sizeof(vec3))) return false;
    // if (memcmp(&obj1->bounds, &obj2->bounds, sizeof(aabb))) return false;
    if (memcmp(&obj1->anim, &obj2->anim, sizeof(anim_t))) return false;
    // if (obj1->anim_speed != obj2->anim_speed) return false;
    if (obj1->billboard != obj2->billboard) return false;
    if (obj1->fullbright != obj2->fullbright) return false;
    if (obj1->checksum != obj2->checksum) return false;
    return true;
}

void object_pivot(object_t *obj, vec3 euler) {
    obj->pivot = euler;
    object_update(obj);
}

void object_rotate(object_t *obj, vec3 euler) {
    quat p = eulerq(vec3(obj->pivot.x,obj->pivot.y,obj->pivot.z));
    quat e = eulerq(vec3(euler.x,euler.y,euler.z));
    obj->rot = mulq(p,e);
    obj->euler = euler;
    object_update(obj);
}

void object_teleport(object_t *obj, vec3 pos) {
    obj->pos = pos;
    object_update(obj);
}

void object_move(object_t *obj, vec3 inc) {
    obj->pos = add3(obj->pos, inc);
    object_update(obj);
}

void object_scale(object_t *obj, vec3 sca) {
    obj->sca = vec3(sca.x, sca.y, sca.z);
    object_update(obj);
}

void object_batchable(object_t *obj, bool batchable) {
    obj->batchable = batchable;
}

vec3 object_position(object_t *obj) {
    return vec3(obj->transform[12], obj->transform[13], obj->transform[14]);
}

void object_model(object_t *obj, model_t model) {
    obj->model = model;
}

void object_model_shadow(object_t *obj, model_t model) {
    obj->model_shadow = model;
}

void object_anim(object_t *obj, anim_t anim, float speed) {
    obj->anim = anim;
    obj->anim_speed = speed;
}

void object_push_diffuse(object_t *obj, texture_t tex) {
    array_push(obj->textures, tex);
}

void object_pop_diffuse(object_t *obj) {
    array_pop(obj->textures);
}

void object_diffuse(object_t *obj, texture_t tex) {
    array_clear(obj->textures);
    object_push_diffuse(obj, tex);
}

void object_billboard(object_t *obj, unsigned mode) {
    obj->billboard = mode;
}

#endif
