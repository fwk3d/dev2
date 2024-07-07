// -----------------------------------------------------------------------------
// anims

#if !CODE

enum ANIM_FLAGS {
    ANIM_LOOP = 1,
    ANIM_DONT_RESET_AFTER_USE = 2,
};

typedef struct anim_t {
    int        from;
    int        to;
    float      blendtime;
    unsigned   flags;
    float      curframe;

    unsigned   easing;
    float      alpha; // refreshed at every tick
    float      timer; // private
    bool       active;

    vec3       pose; // private
    char*      name; // debug
} anim_t;

API anim_t clip(float minframe, float maxframe, float blendtime, unsigned flags);
API anim_t loop(float minframe, float maxframe, float blendtime, unsigned flags);
API array(anim_t) animlist(const char *filename);

// internal
API vec3 pose(bool forward_time, float curframe, int minframe, int maxframe, bool loop, float *retframe);

#else

anim_t clip(float minframe, float maxframe, float blendtime, unsigned flags) {
    return ((anim_t){minframe, maxframe, blendtime, flags, 1e6});
}
anim_t loop(float minframe, float maxframe, float blendtime, unsigned flags) {
    return clip(minframe, maxframe, blendtime, flags | ANIM_LOOP);
}

array(anim_t) animlist(const char *pathfile) {
    anim_t *animlist = 0;
    char *anim_file = file_read(strendi(pathfile,".txt") ? pathfile : va("%s@animlist.txt", pathfile), 0);
    if( anim_file ) {
        // deserialize anim
        for each_substring(anim_file, "\r\n", anim) {
            int from, to;
            char anim_name[128] = {0};
            if( sscanf(anim, "%*s %d-%d %127[^\r\n]", &from, &to, anim_name) != 3) continue;
            array_push(animlist, !!strstri(anim_name, "loop") || !strcmpi(anim_name, "idle") ? loop(from, to, 0, 0) : clip(from, to, 0, 0)); // [from,to,flags]
            array_back(animlist)->name = strcut(strcut(strcut(STRDUP(anim_name), "Loop"), "loop"), "()"); // @leak
        }
    } else {
        // placeholder
        array_push(animlist, clip(0,1,0,0));
        array_back(animlist)->name = STRDUP("Error"); // @leak
    }
    return animlist;
}

vec3 pose(bool forward_time, float curframe, int minframe, int maxframe, bool loop, float *retframe) {
    float offset = curframe - (int)curframe;
#if 1
    int frame1 = (int)curframe;
    int frame2 = frame1 + (forward_time ? 1 : -1);
#else
    float frame1 = curframe;
    float frame2 = curframe + (forward_time ? 1 : -1);
#endif

    if( loop ) {
        int distance = maxframe - minframe;
        frame1 = fmod(frame1 - minframe, distance) + minframe; // frame1 >= maxframe ? minframe : frame1 < minframe ? maxframe - clampf(minframe - frame1, 0, distance) : frame1;
        frame2 = fmod(frame2 - minframe, distance) + minframe; // frame2 >= maxframe ? minframe : frame2 < minframe ? maxframe - clampf(minframe - frame2, 0, distance) : frame2;
        if(retframe) *retframe = fmod(frame1 + offset - minframe, distance) + minframe;
    } else {
        frame1 = clampf(frame1, minframe, maxframe);
        frame2 = clampf(frame2, minframe, maxframe);
        if(retframe) *retframe = clampf(frame1 + offset, minframe, maxframe);
    }

    return vec3(frame1 + (offset > 0 && offset < 1 ? offset : 0),frame2,offset);
}

static
void anim_tick(anim_t *p, bool is_primary, float delta) { // delta can be negative (reverses anim)
    if( !is_primary ) p->active = 0;
    if( is_primary && !p->active ) {
        p->active = 1;
        p->timer = 0;
        p->alpha = 0;
        if( p->flags & ANIM_DONT_RESET_AFTER_USE ) {} else p->curframe = 1e6;
    }

    p->alpha = 1 - ease(p->timer / p->blendtime, p->easing);
    p->timer += window_delta();

    p->curframe += delta;
    if(p->curframe < p->from || p->curframe > p->to ) p->curframe = delta >= 0 ? p->from : p->to;
    p->pose = pose(delta >= 0, p->curframe, p->from, p->to, p->flags & ANIM_LOOP, NULL);
}

#endif
