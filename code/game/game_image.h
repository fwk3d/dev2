#if !CODE

/// flags when constructing the image_t type. see: image, image_from_mem
/// IMAGE_R: 1-channel image (R)
/// IMAGE_RG: 2-channel image (R,G)
/// IMAGE_RGB: 3-channel image (R,G,B)
/// IMAGE_RGBA: 4-channel image (R,G,B,A)
/// IMAGE_FLIP: Flip image vertically
/// IMAGE_FLOAT: Float pixel components
enum IMAGE_FLAGS {
    IMAGE_R     = 0x01000,
    IMAGE_RG    = 0x02000,
    IMAGE_RGB   = 0x04000,
    IMAGE_RGBA  = 0x08000,
    IMAGE_FLIP  = 0x10000,
    IMAGE_FLOAT = 0x20000,
};

typedef struct image_t {
    union {
        struct { unsigned w,h,n; };
        struct { unsigned x,y; };
    };
    void *pixels;
} image_t;

API image_t image(const void *file, unsigned ptrlen, unsigned flags);
API image_t image_create(int x, int y, int flags);
API void    image_destroy(image_t *im);

#else

image_t image_create(int x, int y, int flags) {
    int n = 3; // defaults to RGB
    if(flags & IMAGE_R) n = 1;
    if(flags & IMAGE_RG) n = 2;
    if(flags & IMAGE_RGB) n = 3;
    if(flags & IMAGE_RGBA) n = 4;
    image_t img; img.x = x; img.y = y; img.n = n;
    img.pixels = REALLOC(0, x * y * n ); // @fixme: image_destroy() requires stbi allocator to match REALLOC
    return img;
}

image_t image(const void *data, unsigned size, unsigned flags) {
    if (data == NULL) {
        return ((image_t){0});
    }
    if( !size ) data = file_read((const char *)data, (int*)&size);

    image_t img = {0};
    if( data && size ) {
        stbi_set_flip_vertically_on_load(flags & IMAGE_FLIP ? 1 : 0);

        int n = 0;
        if(flags & IMAGE_R) n = 1;
        if(flags & IMAGE_RG) n = 2;
        if(flags & IMAGE_RGB) n = 3;
        if(flags & IMAGE_RGBA) n = 4;
        if(flags & IMAGE_FLOAT)
        img.pixels = stbi_loadf_from_memory((const stbi_uc*)data, size, (int*)&img.x,(int*)&img.y,(int*)&img.n, n);
        else
        img.pixels = stbi_load_from_memory((const stbi_uc*)data, size, (int*)&img.x,(int*)&img.y,(int*)&img.n, n);
        if( img.pixels ) {
            PRINTF("Loaded image (%dx%d %.*s->%.*s)\n",img.w,img.h,img.n,"RGBA",n?n:img.n,"RGBA");
        } else {
            // die("Error loading image (%s)\n", pathfile);
        }
        img.n = n ? n : img.n;
    }
    return img;
}

void image_destroy(image_t *img) {
    if(img->pixels) stbi_image_free(img->pixels);
    img->pixels = 0; // *img = (image_t){0}; // do not clear fields yet. might be useful in the future.
}

#if 0
// bilinear interpolation (uv must be in image coords, range [0..w-1,0..h-1])
static
vec3 bilinear(image_t in, vec2 uv) { // image_bilinear_pixel() ?
    float w = in.x, h = in.y, u = uv.x, v = uv.y;
    float u1 = (int)u, v1 = (int)v, u2 = minf(u1+1, w-1), v2 = minf(v1+1, h-1);
    float c1 = u - u1, c2 = v - v1;
    uint8_t *p1 = &in.pixels8[ in.n * (int)(u1 + v1 * in.w) ];
    uint8_t *p2 = &in.pixels8[ in.n * (int)(u2 + v1 * in.w) ];
    uint8_t *p3 = &in.pixels8[ in.n * (int)(u1 + v2 * in.w) ];
    uint8_t *p4 = &in.pixels8[ in.n * (int)(u2 + v2 * in.w) ];
    vec3 A = vec3( p1[0], p1[1], p1[2] );
    vec3 B = vec3( p2[0], p2[1], p2[2] );
    vec3 C = vec3( p3[0], p3[1], p3[2] );
    vec3 D = vec3( p4[0], p4[1], p4[2] );
    return mix3(mix3(A, B, c1), mix3(C, D, c1), c2);
}
#endif
#endif
