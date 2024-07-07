// -----------------------------------------------------------------------------
// colormap

#if !CODE

typedef struct colormap_t {
    vec4 color;
    texture_t *texture;
    bool no_mipmaps;
} colormap_t;

API bool colormap( colormap_t *cm, const char *texture_name, bool load_as_srgb );

#else

bool colormap( colormap_t *cm, const char *texture_name, bool load_as_srgb ) {
    if( !texture_name ) return false;

    if( cm->texture && cm->texture->id != texture_checker().id ) {
        return true;
    }

    int srgb = load_as_srgb ? TEXTURE_SRGB : 0;
    int mipmapped = !cm->no_mipmaps ? TEXTURE_MIPMAPS|TEXTURE_ANISOTROPY : 0;
    int hdr = strendi(texture_name, ".hdr") ? TEXTURE_FLOAT|TEXTURE_RGBA : 0;
    texture_t t = texture_compressed(texture_name, TEXTURE_LINEAR | TEXTURE_REPEAT | mipmapped | hdr | srgb);

    if( t.id == texture_checker().id ) {
        cm->texture = NULL;
        return false;
    }
    cm->texture = CALLOC(1, sizeof(texture_t));
    *cm->texture = t;
    return true;
}

#endif
