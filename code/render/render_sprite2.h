#if !CODE

// ----------------------------------------------------------------------------
// atlas api

typedef struct atlas_frame_t {
    unsigned delay;
    vec4 sheet;
    vec2 anchor; // @todo
    array(vec3i) indices;
    array(vec2) coords;
    array(vec2) uvs;
} atlas_frame_t;

typedef struct atlas_anim_t {
    unsigned name;
    array(unsigned) frames;
} atlas_anim_t;

typedef struct atlas_slice_frame_t {
    vec4 bounds;
    bool has_9slice;
    vec4 core;
    vec2 pivot;
    unsigned color;
    char *text;
} atlas_slice_frame_t;

typedef struct atlas_slice_t {
    unsigned name;
    array(unsigned) frames;
} atlas_slice_t;

typedef struct atlas_t {
    texture_t tex;

    array(atlas_frame_t) frames;
    array(atlas_anim_t)  anims;
    array(atlas_slice_t) slices;
    array(atlas_slice_frame_t) slice_frames;

    quarks_t db;
} atlas_t;

API atlas_t atlas_create(const char *inifile, unsigned flags);
API int         ui_atlas(atlas_t *a);
API int         ui_atlas_frame(atlas_frame_t *f);
API void    atlas_destroy(atlas_t *a);

// ----------------------------------------------------------------------------
// sprite v2 api

typedef struct sprite_t { OBJ
    vec4 gamepad; // up,down,left,right
    vec2 fire;    // a,b

    vec4 pos;
    vec2 sca;
    float tilt;
    unsigned tint;
    unsigned frame;
    unsigned timer, timer_ms;
    unsigned flip_, flipped;
    unsigned play;
    bool paused;
    // array(unsigned) play_queue; or unsigned play_next;
    struct atlas_t *a; // shared
    //atlas_t own; // owned
} sprite_t;

API void     sprite_ctor(sprite_t *s);
API void     sprite_dtor(sprite_t *s);
API void     sprite_tick(sprite_t *s);
API void     sprite_draw(sprite_t *s);
API void     sprite_edit(sprite_t *s);

API sprite_t*sprite_new(const char *ase, int bindings[6]);
API void     sprite_del(sprite_t *s);
API void     sprite_setanim(sprite_t *s, unsigned name);

#else

// ----------------------------------------------------------------------------
// atlas

int ui_atlas_frame(atlas_frame_t *f) {
    ui_unsigned("delay", &f->delay);
    ui_vec4("sheet", &f->sheet);
    ui_array("indices", vec3i, &f->indices);
    ui_array("coords", vec2, &f->coords);
    ui_array("uvs", vec2, &f->uvs);
    return 0;
}

int ui_atlas_slice_frame(atlas_slice_frame_t *f) {
    ui_vec4("bounds", &f->bounds);
    ui_bool("9-slice", &f->has_9slice);
    ui_vec4("core", &f->core);
    return 0;
}

int ui_atlas(atlas_t *a) {
    int changed = 0;
    ui_texture(NULL, a->tex);
    for( int i = 0; i < array_count(a->anims); ++i ) {
        if( ui_collapse(quarks_string(&a->db, a->anims[i].name), va("%p%d", a, a->anims[i].name) ) ) {
            changed = i+1;
            for( int j = 0; j < array_count(a->anims[i].frames); ++j ) {
                if( ui_collapse(va("[%d]",j), va("%p%d.%d", a, a->anims[i].name,j) ) ) {
                    ui_unsigned("Frame", &a->anims[i].frames[j]);
                    ui_atlas_frame(a->frames + a->anims[i].frames[j]);
                    ui_collapse_end();
                }
            }
            ui_collapse_end();
        }
    }
    for( int i = 0; i < array_count(a->slices); ++i ) {
        if( ui_collapse(quarks_string(&a->db, a->slices[i].name), va("%p%d", a, a->slices[i].name) ) ) {
            changed = i+1;
            for( int j = 0; j < array_count(a->slices[i].frames); ++j ) {
                if( ui_collapse(va("[%d]",j), va("%p%d.%d", a, a->slices[i].name,j) ) ) {
                    // ui_unsigned("Frame", &a->slices[i].frames[j]);
                    ui_atlas_slice_frame(a->slice_frames + a->slices[i].frames[j]);
                    ui_collapse_end();
                }
            }
            ui_collapse_end();
        }
    }
    return changed;
}

void atlas_destroy(atlas_t *a) {
    if( a ) {
        texture_destroy(&a->tex);
        memset(a, 0, sizeof(atlas_t));
    }
}
atlas_t atlas_create(const char *inifile, unsigned flags) {
    atlas_t a = {0};
    int padding = 0, border = 0;

    #define array_reserve_(arr,x) (array_count(arr) > (x) ? (arr) : array_resize(arr, 1+(x)))

    ini_t kv = ini(inifile);
    for each_map(kv, char*,k, char*,v ) {
        unsigned index = atoi(k);
        // printf("entry %s=%s\n", k, v);
        /**/ if( strend(k, ".name") ) {
            array_reserve_(a.anims, index);

            a.anims[index].name = quarks_intern(&a.db, v);
        }
        else if ( strend(k, ".sl_name") ) {
            array_reserve_(a.slices, index);

            a.slices[index].name = quarks_intern(&a.db, v);
        }
        else if ( strend(k, ".sl_frames") ) {
            array_reserve_(a.slices, index);

            const char *text = v;
            array(char*) frames = strsplit(text, ",");
            for( int i = 0; i < array_count(frames); i++ ) {
                unsigned frame = atoi(frames[i]);
                array_push(a.slices[index].frames, frame);
            }
        }
        else if ( strend(k, ".sl_bounds") ) {
            array_reserve_(a.slice_frames, index);

            float x,y,z,w;
            sscanf(v, "%f,%f,%f,%f", &x, &y, &z, &w);

            a.slice_frames[index].bounds = vec4(x,y,x+z,y+w);
        }
        else if ( strend(k, ".sl_9slice") ) {
            array_reserve_(a.slice_frames, index);

            a.slice_frames[index].has_9slice = atoi(v);
        }
        else if ( strend(k, ".sl_core") ) {
            array_reserve_(a.slice_frames, index);

            float x,y,z,w;
            sscanf(v, "%f,%f,%f,%f", &x, &y, &z, &w);

            a.slice_frames[index].core = vec4(x,y,x+z,y+w);
        }
        else if ( strend(k, ".sl_pivot") ) {
            array_reserve_(a.slice_frames, index);

            float x,y;
            sscanf(v, "%f,%f", &x, &y);

            a.slice_frames[index].pivot = vec2(x,y);
        }
        else if ( strend(k, ".sl_color") ) {
            array_reserve_(a.slice_frames, index);

            unsigned color;
            sscanf(v, "%u", &color);

            a.slice_frames[index].color = color;
        }
        else if ( strend(k, ".sl_text") ) {
            array_reserve_(a.slice_frames, index);
            a.slice_frames[index].text = STRDUP(v);
        }
        else if( strend(k, ".frames") ) {
            array_reserve_(a.anims, index);

            array(char*) pairs = strsplit(v, ",");
            for( int i = 0, end = array_count(pairs); i < end; i += 2 ) {
                unsigned frame = atoi(pairs[i]);
                unsigned delay = atoi(pairs[i+1]);

                array_reserve_(a.frames, frame);
                a.frames[frame].delay = delay;

                array_push(a.anims[index].frames, frame);
            }
        }
        else if( strend(k, ".sheet") ) {
            array_reserve_(a.frames, index);

            vec4 sheet = atof4(v); //x,y,x2+2,y2+2 -> x,y,w,h (for 2,2 padding)
            a.frames[index].sheet = vec4(sheet.x,sheet.y,sheet.z-sheet.x,sheet.w-sheet.y);
        }
        else if( strend(k, ".indices") ) {
            array_reserve_(a.frames, index);

            const char *text = v;
            array(char*) tuples = strsplit(text, ",");
            for( int i = 0, end = array_count(tuples); i < end; i += 3 ) {
                unsigned p1 = atoi(tuples[i]);
                unsigned p2 = atoi(tuples[i+1]);
                unsigned p3 = atoi(tuples[i+2]);
                array_push(a.frames[index].indices, vec3i(p1,p2,p3));
            }
        }
        else if( strend(k, ".coords") ) {
            array_reserve_(a.frames, index);

            const char *text = v;
            array(char*) pairs = strsplit(text, ",");
            for( int i = 0, end = array_count(pairs); i < end; i += 2 ) {
                unsigned x = atoi(pairs[i]);
                unsigned y = atoi(pairs[i+1]);
                array_push(a.frames[index].coords, vec2(x,y));
            }
        }
        else if( strend(k, ".uvs") ) {
            array_reserve_(a.frames, index);

            const char *text = v;
            array(char*) pairs = strsplit(text, ",");
            for( int i = 0, end = array_count(pairs); i < end; i += 2 ) {
                unsigned u = atoi(pairs[i]);
                unsigned v = atoi(pairs[i+1]);
                array_push(a.frames[index].uvs, vec2(u,v));
            }
        }
        else if( strend(k, "padding") ) {
            padding = atoi(v);
        }
        else if( strend(k, "border") ) {
            border = atoi(v);
        }
        else if( strend(k, "file") ) {
            a.tex = texture(v, 0);
        }
        else if( strend(k, "bitmap") ) {
            const char *text = v;
            array(char) bin = base64_decode(text, strlen(text));
            a.tex = texture_from_mem(bin, array_count(bin), 0);
            array_free(bin);
        }
#if 0
        else if( strend(k, ".frame") ) {
            array_reserve_(a.frames, index);
            puts(k), puts(v);
        }
#endif
    }

    // post-process: normalize uvs and coords into [0..1] ranges
    for each_array_ptr(a.frames, atlas_frame_t, f) {
        for each_array_ptr(f->uvs, vec2, uv) {
            uv->x /= a.tex.w;
            uv->y /= a.tex.h;
        }
        for each_array_ptr(f->coords, vec2, xy) {
            xy->x /= a.tex.w;
            xy->y /= a.tex.h;
        }
        // @todo: adjust padding/border
    }
    for each_array_ptr(a.slice_frames, atlas_slice_frame_t, f) {
        f->bounds.x += padding+border;
        f->bounds.y += padding+border;
        f->bounds.z += padding+border;
        f->bounds.w += padding+border;
    }
#if 0
    // post-process: specify an anchor for each anim based on 1st frame dims
    for each_array_ptr(a.anims, atlas_anim_t, anim) {
        atlas_frame_t *first = a.frames + *anim->frames;
        for( int i = 0; i < array_count(anim->frames); i += 2) {
            atlas_frame_t *ff = a.frames + anim->frames[ i ];
            ff->anchor.x = (ff->sheet.z - first->sheet.z) / 2;
            ff->anchor.y = (ff->sheet.w - first->sheet.w) / 2;
        }
    }
#endif

    #undef array_reserve_

    return a;
}

// ----------------------------------------------------------------------------
// sprite v2

void sprite_ctor(sprite_t *s) {
    s->tint = WHITE;
    s->timer_ms = 100;
    s->flipped = 1;
    s->sca.x += !s->sca.x;
    s->sca.y += !s->sca.y;
}
void sprite_dtor(sprite_t *s) {
    memset(s, 0, sizeof(*s));
}
void sprite_tick(sprite_t *s) {
    int right = input(s->gamepad.array[3]) - input(s->gamepad.array[2]); // RIGHT - LEFT
    int forward = input(s->gamepad.array[1]) - input(s->gamepad.array[0]); // DOWN - UP
    int move = right || forward;
    int dt = 16; // window_delta() * 1000;

    unsigned over = (s->timer - dt) > s->timer;
    if(!s->paused) s->timer -= dt;
    if( over ) {
        int len = array_count(s->a->anims[s->play].frames);
        unsigned next = (s->frame + 1) % (len + !len);
        unsigned eoa = next < s->frame;
        s->frame = next;

        atlas_frame_t *f = &s->a->frames[ s->a->anims[s->play].frames[s->frame] ];
        s->timer_ms = f->delay;
        s->timer += s->timer_ms;
    }

    if( s->play == 0 && move ) sprite_setanim(s, 1);
    if( s->play == 1 ) { //<
        if(right) s->flip_ = right < 0, sprite_setanim(s, 1);
        if(!right && !forward) sprite_setanim(s, 0);

        float speed = s->sca.x*2;
        s->pos = add4(s->pos, scale4(norm4(vec4(right,0,forward,0)),speed));
    }
}
void sprite_draw(sprite_t *s) {
    atlas_frame_t *f = &s->a->frames[ s->a->anims[s->play].frames[s->frame] ];

#if 1
    // @todo {
        unsigned sample = s->a->anims[s->play].frames[s->frame];
        sample = 0;
        f->anchor.x = (-s->a->frames[sample].sheet.z + f->sheet.z) / 2;
        f->anchor.y = (+s->a->frames[sample].sheet.w - f->sheet.w) / 2;
    // }
#endif

    // rect(x,y,w,h) is [0..1] normalized, z-index, pos(x,y,scale), rotation (degrees), color (rgba)
    vec4 rect = { f->sheet.x / s->a->tex.w, f->sheet.y / s->a->tex.h, f->sheet.z / s->a->tex.w, f->sheet.w / s->a->tex.h };
    sprite_rect(s->a->tex, rect, s->pos, vec4(s->flip_ ^ s->flipped?s->sca.x:-s->sca.x,s->sca.y,f->anchor.x,f->anchor.y), s->tilt, s->tint, 0|SPRITE_PROJECTED);
}
void sprite_edit(sprite_t *s) {
    const char *name = obj_name(s);
    const char *id = va("%p", s);
    if( s && ui_collapse(name ? name : id, id) ) {
        ui_obj("%s", (obj*)s);

        ui_bool("paused", &s->paused);
        ui_label(va("frame anim [%d]", s->a->anims[s->play].frames[s->frame]));

        int k = s->play;
        if( ui_int("anim", &k) ) {
            sprite_setanim(s, k);
        }

        int selected = ui_atlas(s->a);
        if( selected ) sprite_setanim(s, selected - 1);

        ui_collapse_end();
    }
}

OBJTYPEDEF(sprite_t,OBJTYPE_sprite);

sprite_t* sprite_new(const char *ase, int bindings[6]) {
    sprite_t *s = obj_new(sprite_t, {bindings[0],bindings[1],bindings[2],bindings[3]}, {bindings[4],bindings[5]});
    atlas_t own = atlas_create(ase, 0);
    memcpy(s->a = MALLOC(sizeof(atlas_t)), &own, sizeof(atlas_t)); // s->a = &s->own;
    return s;
}
void sprite_del(sprite_t *s) {
    if( s ) {
        if( s->a ) atlas_destroy(s->a), FREE(s->a); // if( s->a == &s->own )
        obj_free(s);
        memset(s, 0, sizeof(sprite_t));
    }
}
void sprite_setanim(sprite_t *s, unsigned name) {
    if( s->play != name ) {
        s->play = name;
        s->frame = 0;

        atlas_frame_t *f = &s->a->frames[ s->a->anims[s->play].frames[s->frame] ];

        s->timer_ms = f->delay;
        s->timer = s->timer_ms;
    }
}

AUTORUN {
    STRUCT(sprite_t, vec4, pos);
    STRUCT(sprite_t, vec2, sca);
    STRUCT(sprite_t, float, tilt);
    STRUCT(sprite_t, vec4, gamepad);
    STRUCT(sprite_t, vec2, fire);
    STRUCT(sprite_t, rgba,  tint);
    STRUCT(sprite_t, unsigned, frame);
    STRUCT(sprite_t, unsigned, timer);
    STRUCT(sprite_t, unsigned, timer_ms);
    STRUCT(sprite_t, unsigned, flipped);
    STRUCT(sprite_t, unsigned, play);
    EXTEND_T(sprite, ctor,edit,draw,tick);
}

#endif
