// -----------------------------------------------------------------------------
// scene

#if !CODE

enum SCENE_FLAGS {
    SCENE_WIREFRAME = 1,
    SCENE_CULLFACE = 2,
    SCENE_BACKGROUND = 4,
    SCENE_FOREGROUND = 8,
    SCENE_UPDATE_SH_COEF = 16,
    SCENE_SHADOWS = 32,
    SCENE_POSTFX = 64,
    SCENE_DRAWMAT = 128,
    // SCENE_DISABLE_BATCHING = 64,
};

typedef struct scene_t {
    array(object_t) objs;
    array(light_t) lights;

    // special objects below:
    skybox_t skybox;
    int u_coefficients_sh;
    shadowmap_t shadowmap;
    drawmat_t drawmat;
} scene_t;

API scene_t*  scene_push();
API void      scene_pop();
API scene_t*  scene_get_active();

API int       scene_merge(const char *source);
API void      scene_render(int flags);

API object_t* scene_spawn();
API unsigned  scene_count();
API object_t* scene_index(unsigned index);

API light_t*  scene_spawn_light();
API void      scene_merge_lights(const char *source);
API unsigned  scene_count_light();
API light_t*  scene_index_light(unsigned index);
API void      scene_skybox(skybox_t sky);

#else
// -----------------------------------------------------------------------------

array(scene_t*) scenes;
scene_t* last_scene;

static void scene_init() {
#ifndef __EMSCRIPTEN__ // @fixme ems -> shaders
    do_once scene_push();      // create an empty scene by default
#endif
}

scene_t* scene_get_active() {
    return last_scene;
}

scene_t* scene_push() {
    scene_t *s = REALLOC(0, sizeof(scene_t)), clear = {0}; *s = clear;
    s->skybox = skybox(NULL, 0);
    array_push(scenes, s);
    last_scene = s;
    return s;
}

void scene_pop() {
    // @fixme: fix leaks, scene_cleanup();
    scene_t clear = {0};
    *last_scene = clear;
    array_pop(scenes);
    last_scene = *array_back(scenes);
}

int scene_merge(const char *source) {
    int count = 0;
    if( json_push(source) ) {
        for(int i = 0, e = json_count("/") - 1; i <= e ; ++i) {
            const char *skybox_folder = json_string("/[%d]/skybox",i);
            if( skybox_folder[0] ) {
                PRINTF("Loading skybox folder: %s\n", skybox_folder);
                last_scene->skybox = skybox( skybox_folder, 0 );
                continue;
            }
            const char *mesh_file = json_string("/[%d]/mesh",i);
            const char *texture_file = json_string("/[%d]/texture",i);
            const char *animation_file = json_string("/[%d]/animation",i);
            vec3 position = vec3(json_float("/[%d]/position[0]",i),json_float("/[%d]/position[1]",i),json_float("/[%d]/position[2]",i));
            vec3 rotation = vec3(json_float("/[%d]/rotation[0]",i),json_float("/[%d]/rotation[1]",i),json_float("/[%d]/rotation[2]",i));
            vec3 scale = scale3(vec3(1,1,1), json_float("/[%d]/scale",i));
            bool opt_swap_zy = json_int("/[%d]/swapzy",i);
            bool opt_flip_uv = json_int("/[%d]/flipuv",i);
            bool opt_fullbright = json_int("/[%d]/fullbright",i);
            PRINTF("Scene %d/%d Loading: %s\n", i, e, mesh_file);
            PRINTF("Scene %d/%d Texture: %s\n", i, e, texture_file);
            PRINTF("Scene %d/%d Animation: %s\n", i, e, animation_file);
            PRINTF("Scene %d/%d Position: (%f,%f,%f)\n", i, e, position.x, position.y, position.z);
            PRINTF("Scene %d/%d Rotation: (%f,%f,%f)\n", i, e, rotation.x, rotation.y, rotation.z);
            PRINTF("Scene %d/%d Scale: (%f,%f,%f)\n", i, e, scale.x, scale.y, scale.z);
            PRINTF("Scene %d/%d Swap_ZY: %d\n", i, e, opt_swap_zy );
            PRINTF("Scene %d/%d Flip_UV: %d\n", i, e, opt_flip_uv );
            model_t m = model(mesh_file, 0);
            //char *a = archive_read(animation_file);
            object_t *o = scene_spawn();
            object_model(o, m);
            if( texture_file[0] ) object_diffuse(o, texture(texture_file, (opt_flip_uv ? IMAGE_FLIP : 0)) );
            object_scale(o, scale);
            object_teleport(o, position);
            object_pivot(o, rotation); // object_rotate(o, rotation);
            o->fullbright = opt_fullbright;
            //object_name(x), scene_find(name)
// o->bounds = aabb(mul3(m.bounds.min,o->sca),mul3(m.bounds.max,o->sca));
// PRINTF("aabb={%f,%f,%f},{%f,%f,%f}\n", o->bounds.min.x, o->bounds.min.y, o->bounds.min.z, o->bounds.max.x, o->bounds.max.y, o->bounds.max.z);

/*
            if(opt_swap_zy) {
                // swap zy bounds
                vec3 min = o->bounds.min, max = o->bounds.max;
                o->bounds = aabb( vec3(min.x,min.z,min.y), vec3(max.x,max.z,max.y) );
            }
*/

            count++;
        }
        json_pop();
    }
    // PRINTF("scene loading took %5.2fs\n", secs);
    PRINTF("%d objects merged into scene\n", count);
    return count;
}

object_t* scene_spawn() {
    object_t obj = object();
    array_push(last_scene->objs, obj);

    return array_back(last_scene->objs);
}

unsigned scene_count() {
    return array_count(last_scene->objs);
}

object_t* scene_index(unsigned obj_index) {
    unsigned obj_count = scene_count();
    ASSERT(obj_index < obj_count, "Object index %d exceeds number (%d) of spawned objects", obj_index, obj_count);
    return &last_scene->objs[obj_index];
}

light_t* scene_spawn_light() {
    light_t l = light();
    array_push(last_scene->lights, l);

    return array_back(last_scene->lights);
}

void scene_merge_lights(const char *source) {
    light_t *lights = lightlist(source);
    for (unsigned i = 0; i < array_count(lights); ++i) {
        light_t *l = &lights[i];
        array_push(last_scene->lights, *l);
    }
    array_free(lights);
}

unsigned scene_count_light() {
    return array_count(last_scene->lights);
}

light_t* scene_index_light(unsigned light_index) {
    unsigned light_count = scene_count_light();
    ASSERT(light_index < light_count, "Light index %d exceeds number (%d) of spawned lights", light_index, light_count);
    return &last_scene->lights[light_index];
}

void scene_skybox(skybox_t sky) {
    skybox_destroy(&last_scene->skybox);
    last_scene->skybox = sky;
}

static
int scene_obj_distance_compare(const void *a, const void *b) {
    const object_t *da = a, *db = b;
    return da->distance < db->distance ? 1 : da->distance > db->distance ? -1 : 0;
}

void scene_render(int flags) {
    camera_t *cam = camera_get_active();

    shadowmap_t *sm = &last_scene->shadowmap;

    if (flags & SCENE_POSTFX) {
        fx_begin();
    }

    if (flags & SCENE_SHADOWS) {
        if (sm->vsm_texture_width == 0) {
            *sm = shadowmap(DEFAULT_SHADOW_VSM_RESOLUTION, DEFAULT_SHADOW_CSM_RESOLUTION);
        }
    } else {
        sm = NULL;
    }

    if(flags & SCENE_BACKGROUND) {
        if(last_scene->skybox.program) {
            skybox_push_state(&last_scene->skybox, cam->proj, cam->view);
            mesh_render(&last_scene->skybox.geometry);
            skybox_pop_state();
        }

        ddraw_flush();
    }

    if( flags & SCENE_FOREGROUND ) {
        bool do_relighting = 0;
        for (unsigned j = 0; j < array_count(last_scene->lights); ++j) {
            if (!last_scene->lights[j].cached) {
                do_relighting = 1;
                break;
            }
        }

        for(unsigned j = 0, obj_count = scene_count(); j < obj_count; ++j ) {
            object_t *obj = scene_index(j);
            model_t *model = &obj->model;
            obj->was_batched = false;
            // array_resize(obj->pair_instance, obj_count);

            model->billboard = obj->billboard;
            for (int p = 0; p < RENDER_PASS_OVERRIDES_BEGIN; ++p) {
                model->rs[p].cull_face_enabled = flags&SCENE_CULLFACE ? 1 : 0;
                model->rs[p].polygon_mode_draw = flags&SCENE_WIREFRAME ? GL_LINE : GL_FILL;
            }
            obj->checksum = obj->model.iqm ? model_checksum(&obj->model) : 0;
        }

        for(unsigned j = 0, obj_count = scene_count(); j < obj_count; ++j ) {
            object_t *obj = scene_index(j);
            model_t *model = &obj->model;
            anim_t *anim = &obj->anim;
            mat44 *views = (mat44*)(&cam->view);

            if (obj->skip_draw) continue;
            if (obj->was_batched) continue;

            if (anim) {
                float delta = window_delta() * obj->anim_speed;
                model->curframe = model_animate_clip(*model, model->curframe + delta, anim->from, anim->to, anim->flags & ANIM_LOOP );
            }

            if (!obj->fullbright) {
                model_skybox(model, last_scene->skybox);
            } else {
                skybox_t sb = {0};
                model_skybox(model, sb);
            }
            
            array_resize(obj->instances, 64);
            copy44(obj->instances[0], obj->transform);

            int num_instances = 1;
            for (unsigned k = j+1; k < obj_count; ++k) {
                object_t *obj2 = scene_index(k);
                if (!obj2->batchable || obj2->skip_draw || !object_compare(obj, obj2)) {
                    continue;
                }
                if (num_instances >= array_count(obj->instances)) {
                    array_resize(obj->instances, array_count(obj->instances) + 64);
                }
                copy44(obj->instances[num_instances++], obj2->transform);
                obj2->was_batched = true;
            }
            obj->num_instances = num_instances;
            if ( do_relighting || !obj->light_cached ) {
                obj->light_cached = 1;
                // shader_bind(model->iqm->program);
                // light_update(array_count(last_scene->lights), last_scene->lights);
                // @todo: rework light caching
            }
        }

        /* Build shadowmaps */
        if (flags & SCENE_SHADOWS) { 
            shadowmap_begin(sm);
            for (unsigned j = 0; j < array_count(last_scene->lights); ++j) {
                light_t *l = &last_scene->lights[j];
                while (shadowmap_step(sm)) {
                    shadowmap_light(sm, l, cam->proj, cam->view);
                    if (!l->processed_shadows) continue;
                    for(unsigned j = 0, obj_count = scene_count(); j < obj_count; ++j ) {
                        object_t *obj = scene_index(j);
                        model_t *model = obj->model_shadow.iqm ? &obj->model_shadow : &obj->model;
                        if (obj->model.iqm && obj->cast_shadows && !obj->was_batched) {
                            model_render(model, cam->proj, cam->view, obj->instances, obj->num_instances, RENDER_PASS_SHADOW);
                        }
                    }
                }
            }
            shadowmap_end(sm);
        }

        if (flags & SCENE_DRAWMAT) {
            drawmat_clear(&last_scene->drawmat);

            for(unsigned j = 0, obj_count = scene_count(); j < obj_count; ++j ) {
                object_t *obj = scene_index(j);
                model_t *model = &obj->model;
                if (!model->iqm) continue;
                if (obj->skip_draw) continue;
                if (obj->was_batched) continue;
                drawmat_render(&last_scene->drawmat, *model, cam->proj, cam->view, obj->instances, obj->num_instances);
            }
        }

        /* Collect all transparency enabled models and sort them by distance */
        static array(object_t*) transparent_objects = 0;
        for(unsigned j = 0, obj_count = scene_count(); j < obj_count; ++j ) {
            object_t *obj = scene_index(j);
            model_t *model = &obj->model;

            if (!model->iqm) continue;
            if (obj->skip_draw) continue;
            if (obj->was_batched) continue;

            if (model_has_transparency(*model)) {
                obj->distance = len3sq(sub3(cam->position, transform344(model->pivot, obj->pos)));
                array_push(transparent_objects, obj);
            }
        }

        array_sort(transparent_objects, scene_obj_distance_compare);

        /* Opaque pass */
        for(unsigned j = 0, obj_count = scene_count(); j < obj_count; ++j ) {
            object_t *obj = scene_index(j);
            model_t *model = &obj->model;
            if (!model->iqm) continue;
            if (obj->skip_draw) continue;
            if (obj->was_batched) continue;

            model_shadow(model, sm);
            model_light(model, array_count(last_scene->lights), last_scene->lights);
            model_render(model, cam->proj, cam->view, obj->instances, obj->num_instances, RENDER_PASS_OPAQUE);
        }

        /* Transparency pass */
        for (unsigned j = 0; j < array_count(transparent_objects); ++j) {
            object_t *obj = transparent_objects[j];
            model_t *model = &obj->model;
            if (!model->iqm) continue;
            if (obj->skip_draw) continue;
            if (obj->was_batched) continue;

            model_shadow(model, sm);
            model_light(model, array_count(last_scene->lights), last_scene->lights);
            model_render(model, cam->proj, cam->view, obj->instances, obj->num_instances, RENDER_PASS_TRANSPARENT);
        }

        array_resize(transparent_objects, 0);
        glBindVertexArray(0);
    }

    if (flags & SCENE_POSTFX) {
        fx_end(0,0);
    }
}

#endif
