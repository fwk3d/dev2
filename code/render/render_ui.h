API int ui_texture(const char *label, texture_t t);
API int ui_subtexture(const char *label, texture_t t, float u0, float v0, float u1, float v1);
API int ui_colormap( const char *label, colormap_t *cm );

#if CODE

int ui_texture(const char *label, texture_t t) {
    return ui_subimage(label, t.id, t.w,t.h, 0,0,1,1);
}

int ui_subtexture(const char *label, texture_t t, float u0, float v0, float u1, float v1) {
    return ui_subimage(label, t.id, t.w,t.h, u0,v0,u1,v1);
}

int ui_colormap( const char *label, colormap_t *cm ) {
#if 0
    if( label && ui_filter && ui_filter[0] ) if( !strstri(label, ui_filter) ) return 0;
#endif

    int ret = 0;
    if( !cm->texture ) {
        const char *title = va("%s (no image)", label);
        if( ui_image( title, texture_checker().id, 0,0 ) ) {
            ret = 2;
        }
    } else {
        unsigned w = cm->texture->w, h = cm->texture->h;
        ui_label(va("%s (%s)", label, cm->texture->filename) ); // @fixme: labelf would crash?

        const char *fmt[] = { "", "R", "RG", "RGB", "RGBA" };
        const char *title = va("%s %dx%d %s", label, w, h, fmt[cm->texture->n]);
        if( ui_image( title, cm->texture->id, 128, 128 ) ) {
            ret = 2;
        }
    }

    if( ui_color4f( va("%s Color", label), (float *) &cm->color ) ) {
        ret = 1;
    }
    return ret;
}

#endif
