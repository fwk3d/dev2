// ----------------------------------------------------------------------------
// tween

#if !CODE

typedef struct tween_keyframe_t {
    float t;
    vec3 v;
    unsigned ease;
} tween_keyframe_t;

typedef struct tween_t {
    array(tween_keyframe_t) keyframes;

    vec3 result;
    float time;
    float duration;
} tween_t;

API tween_t tween();
API void      tween_setkey(tween_t *tw, float t, vec3 v, unsigned easing_mode);
API void        tween_delkey(tween_t *tw, float t);
API float     tween_update(tween_t *tw, float dt);
API void      tween_reset(tween_t *tw);
API void    tween_destroy(tween_t *tw);

#else

tween_t tween() {
    tween_t tw = {0};
    return tw;
}

float tween_update(tween_t *tw, float dt) {
    if( !array_count(tw->keyframes) ) return 0.0f;

    for( int i = 0, end = array_count(tw->keyframes) - 1; i < end; ++i ) {
        tween_keyframe_t *kf1 = &tw->keyframes[i];
        tween_keyframe_t *kf2 = &tw->keyframes[i + 1];
        if (tw->time >= kf1->t && tw->time <= kf2->t) {
            float localT = (tw->time - kf1->t) / (kf2->t - kf1->t);
            float easedT = ease(localT, kf1->ease);
            tw->result = mix3(kf1->v, kf2->v, easedT);
            break;
        }
    }

    float done = (tw->time / tw->duration);
    tw->time += dt;
    return clampf(done, 0.0f, 1.0f);
}

void tween_reset(tween_t *tw) {
    tw->time = 0.0f;
}

void tween_destroy(tween_t *tw) {
    tween_t tw_ = {0};
    array_free(tw->keyframes);
    *tw = tw_;
}

static
int tween_comp_keyframes(const void *a, const void *b) {
    float t1 = ((const tween_keyframe_t*)a)->t;
    float t2 = ((const tween_keyframe_t*)b)->t;
    return (t1 > t2) - (t1 < t2);
}

void tween_setkey(tween_t *tw, float t, vec3 v, unsigned mode) {
    tween_keyframe_t keyframe = { t, v, mode };
    array_push(tw->keyframes, keyframe);
    array_sort(tw->keyframes, tween_comp_keyframes);
    tw->duration = array_back(tw->keyframes)->t;
}

void tween_delkey(tween_t *tw, float t) { // @todo: untested
    for( int i = 0, end = array_count(tw->keyframes); i < end; i++ ) {
        if( tw->keyframes[i].t == t ) {
            array_erase_slow(tw->keyframes, i);
            tw->duration = array_back(tw->keyframes)->t;
            return;
        }
    }
}

#endif
