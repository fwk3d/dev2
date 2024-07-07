API bool      texture_rec_begin(texture_t *t, unsigned w, unsigned h); // texture_rec
API void      texture_rec_end(texture_t *t); // texture_rec

#if CODE

bool texture_rec_begin(texture_t *t, unsigned tw, unsigned th) {
    for( unsigned w = tw ? tw : window_width(), h = th ? th : window_height(); w*h ; ) {
        // resize if needed
        if( t->w != w || t->h != h ) {
            // re-create texture, set texture parameters and content
            texture_update(t, w, h, 4, NULL, TEXTURE_RGBA);
            if(!t->fbo) t->fbo = fbo_id(t->id, 0, 0);
        }
        // bind fbo to texture
        fbo_bind(t->fbo);
        return true;
    }
    return false;

}
void texture_rec_end(texture_t *t) {
    fbo_unbind();
}

#endif
