// -----------------------------------------------------------------------------
// textures

#if !CODE

typedef unsigned handle; // GLuint

enum TEXTURE_FLAGS {
    // UNIT[0..7]

    TEXTURE_BC1 = 8,  // DXT1, RGB with 8:1 compression ratio (+ optional 1bpp for alpha)
    TEXTURE_BC2 = 16, // DXT3, RGBA with 4:1 compression ratio (BC1 for RGB + 4bpp for alpha)
    TEXTURE_BC3 = 32, // DXT5, RGBA with 4:1 compression ratio (BC1 for RGB + BC4 for A)
//  TEXTURE_BC4,     // Alpha

    TEXTURE_NEAREST = 0,
    TEXTURE_LINEAR = 64,
    TEXTURE_MIPMAPS = 128,
    TEXTURE_ANISOTROPY = 1 << 30,

    TEXTURE_CLAMP = 0,
    TEXTURE_BORDER = 0x100,
    TEXTURE_REPEAT = 0x200,

    TEXTURE_BYTE = 0,
    TEXTURE_FLOAT = IMAGE_FLOAT,

    TEXTURE_COLOR = 0,
    TEXTURE_DEPTH = 0x800,

    TEXTURE_R = IMAGE_R,
    TEXTURE_RG = IMAGE_RG,
    TEXTURE_RGB = IMAGE_RGB,
    TEXTURE_RGBA = IMAGE_RGBA,
    TEXTURE_FLIP = IMAGE_FLIP,

    TEXTURE_UNIQUE = 1 << 31,

    // @fixme
    TEXTURE_SRGB = 1 << 24,
    TEXTURE_BGR = 1 << 25,
    TEXTURE_BGRA = TEXTURE_BGR,
    TEXTURE_ARRAY = 1 << 26,
};

typedef struct texture_t {
    union { unsigned x, w; };
    union { unsigned y, h; };
    union { unsigned z, d; };
    union { unsigned n, bpp; };
    handle id;
    unsigned texel_type;
    unsigned flags;
    char* filename;
    bool transparent;
    bool mipmaps_generated;
    unsigned fbo; // for texture recording
    union { unsigned userdata, delay; };
} texture_t;

API texture_t texture_compressed(const char *filename, unsigned flags);
API texture_t texture_compressed_from_mem(const void *data, int len, unsigned flags);

API texture_t texture(const char* filename, int flags);
API texture_t texture_from_mem(const void* ptr, int len, int flags);
API texture_t texture_create(unsigned w, unsigned h, unsigned n, const void *pixels, int flags);
API texture_t texture_checker();
API void      texture_destroy(texture_t *t);
API unsigned  texture_update(texture_t *t, unsigned w, unsigned h, unsigned n, const void *pixels, int flags);
API void      texture_params(texture_t *t, unsigned flags);

#else

// -----------------------------------------------------------------------------
// textures

unsigned texture_update(texture_t *t, unsigned w, unsigned h, unsigned n, const void *pixels, int flags) {
    if( t && !t->id ) {
        glGenTextures( 1, &t->id );
        return texture_update(t, w, h, n, pixels, flags);
    }
    ASSERT( t && t->id );
    ASSERT( n <= 4 );
    GLuint pixel_types[] = { GL_RED, GL_RED, GL_RG, GL_RGB, GL_RGBA, GL_R32F, GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F };
    GLenum pixel_storage = flags & TEXTURE_FLOAT ? GL_FLOAT : GL_UNSIGNED_BYTE;
    GLuint pixel_type = pixel_types[ n ];
    GLuint texel_type = t->texel_type = pixel_types[ n + 5 * !!(flags & TEXTURE_FLOAT) ];

    if( flags & TEXTURE_BGR )  if( pixel_type == GL_RGB )  pixel_type = GL_BGR;
    if( flags & TEXTURE_BGR )  if( pixel_type == GL_RGBA ) pixel_type = GL_BGRA;
    if( flags & TEXTURE_SRGB ) if( texel_type == GL_RGB )  texel_type = GL_SRGB;
    if( flags & TEXTURE_SRGB ) if( texel_type == GL_RGBA ) texel_type = GL_SRGB_ALPHA; // GL_SRGB8_ALPHA8 ?

    if( flags & TEXTURE_BC1 ) texel_type = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    if( flags & TEXTURE_BC2 ) texel_type = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    if( flags & TEXTURE_BC3 ) texel_type = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    if( flags & TEXTURE_DEPTH ) texel_type = pixel_type = GL_DEPTH_COMPONENT; // GL_DEPTH_COMPONENT32

#if 0
    if( 0 ) { // flags & TEXTURE_PREMULTIPLY_ALPHA )
        uint8_t *p = pixels;
        if(n == 2) for( unsigned i = 0; i < 2*w*h; i += 2 ) {
            p[i] = (p[i] * p[i+1] + 128) >> 8;
        }
        if(n == 4) for( unsigned i = 0; i < 4*w*h; i += 4 ) {
            p[i+0] = (p[i+0] * p[i+3] + 128) >> 8;
            p[i+1] = (p[i+1] * p[i+3] + 128) >> 8;
            p[i+2] = (p[i+2] * p[i+3] + 128) >> 8;
        }
    }
#endif

GLenum texture_type = t->flags & TEXTURE_ARRAY ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D; // @fixme: test GL_TEXTURE_2D_ARRAY

//glPixelStorei( GL_UNPACK_ALIGNMENT, n < 4 ? 1 : 4 ); // for framebuffer reading
//glActiveTexture(GL_TEXTURE0 + (flags&7));
    glBindTexture(texture_type, t->id);
    glTexImage2D(texture_type, 0, texel_type, w, h, 0, pixel_type, pixel_storage, pixels);
    
    texture_params(t, flags);

    // glBindTexture(texture_type, 0); // do not unbind. current code expects texture to be bound at function exit

    t->w = w;
    t->h = h;
    t->n = n;
    t->flags = flags;
    t->filename = t->filename ? t->filename : "";
    t->transparent = 0;

    if (t->n == 4 && pixels) {
        for (int i = 0; i < w * h; i++) {
            if (((uint8_t *)pixels)[i * 4 + 3] < 255) {
                t->transparent = 1;
                break;
            }
        }
    }

    return t->id;
}

void texture_params(texture_t *t, unsigned flags) {    
    ASSERT( t && t->id );
    GLenum wrap = GL_CLAMP_TO_EDGE;
    GLenum min_filter = GL_NEAREST, mag_filter = GL_NEAREST;

    if( flags & TEXTURE_REPEAT ) wrap = GL_REPEAT;
    if( flags & TEXTURE_BORDER ) wrap = GL_CLAMP_TO_BORDER;
    if( flags & TEXTURE_LINEAR ) min_filter = GL_LINEAR, mag_filter = GL_LINEAR;
    if( flags & TEXTURE_MIPMAPS  ) min_filter = flags & TEXTURE_LINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR; // : GL_LINEAR_MIPMAP_NEAREST; maybe?
    if( flags & TEXTURE_MIPMAPS  ) mag_filter = flags & TEXTURE_LINEAR ? GL_LINEAR : GL_NEAREST;
    GLenum texture_type = t->flags & TEXTURE_ARRAY ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D; // @fixme: test GL_TEXTURE_2D_ARRAY

    glBindTexture(texture_type, t->id);
    glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, wrap);
    glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, mag_filter);

    if (flags & TEXTURE_ANISOTROPY) {
        GLfloat value, max_anisotropy = 16.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &value);

        value = (value > max_anisotropy) ? max_anisotropy : value;
        glTexParameterf(texture_type, GL_TEXTURE_MAX_ANISOTROPY, value);
    }

#if 0 // only for sampler2DShadow
    if( flags & TEXTURE_DEPTH )   glTexParameteri(texture_type, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    if( flags & TEXTURE_DEPTH )   glTexParameteri(texture_type, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
#endif
//  if( flags & TEXTURE_BORDER )  glTexParameterfv(texture_type, GL_TEXTURE_BORDER_COLOR, border_color);
    if( flags & TEXTURE_MIPMAPS && !t->mipmaps_generated ) {
        glGenerateMipmap(texture_type);
        t->mipmaps_generated = true;
    }

    if( flags & TEXTURE_MIPMAPS ) {
        GLfloat max_aniso = 0;
        //        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &max_aniso);
        max_aniso = 4;
        glTexParameterf(texture_type, GL_TEXTURE_MAX_ANISOTROPY, max_aniso);
    }
}

texture_t texture_create(unsigned w, unsigned h, unsigned n, const void *pixels, int flags) {
    texture_t texture = {0};
    glGenTextures( 1, &texture.id );
    texture_update( &texture, w, h, n, pixels, flags );
    return texture;
}

texture_t texture_checker() {
    static texture_t texture = {0};
    if( !texture.id ) {
#if 0
        float pixels[] = { 1,0.5,0.5,1 };
        texture = texture_create(2,2,1, pixels, TEXTURE_FLOAT|TEXTURE_MIPMAPS|TEXTURE_REPEAT|TEXTURE_BORDER);
#else
        uint32_t *pixels = REALLOC(0, 256*256*4);
        for (int y = 0, i = 0; y < 256; y++) {
            for (int x = 0; x < 256; x++) {
#if 0
                extern const uint32_t secret_palette[32];
                uint32_t rgb = secret_palette[ y / 8 ] * !!((x ^ y) & 0x8);
                pixels[i++] = (rgb>>16) & 255;
                pixels[i++] = (rgb>>8) & 255;
                pixels[i++] = (rgb>>0) & 255;
                pixels[i++] = 255;
#elif 0
                extern const uint32_t secret_palette[32];
                uint32_t rgb = ((x ^ y) & 0x8) ? secret_palette[6] : secret_palette[ 8 + ((x^y) / (256/6)) ];
                pixels[i++] = (rgb>>16) & 255;
                pixels[i++] = (rgb>>8) & 255;
                pixels[i++] = (rgb>>0) & 255;
                pixels[i++] = 255;
#elif 0
                extern const uint32_t secret_palette[32];
                uint32_t lum = (x^y) & 8 ? 128 : (x^y) & 128 ? 192 : 255;
                uint32_t rgb = rgba(lum,lum,lum,255);
                pixels[i++] = rgb;
#else
                int j = y, i = x;
                unsigned char *p = (unsigned char *)&pixels[x + y * 256];
                p[0] = (i / 16) % 2 == (j / 16) % 2 ? 255 : 0; // r
                p[1] = ((i - j) / 16) % 2 == 0 ? 255 : 0; // g
                p[2] = ((i + j) / 16) % 2 == 0 ? 255 : 0; // b
                p[3] = 255; // a
#endif
            }
        }
        texture = texture_create(256,256,4, pixels, TEXTURE_RGBA|TEXTURE_MIPMAPS|TEXTURE_REPEAT|TEXTURE_BORDER);
        FREE(pixels);
#endif
    }
    return texture;
}

texture_t texture_from_mem(const void *ptr, int len, int flags) {
    image_t img = image(ptr, len, flags);
    if( img.pixels ) {
        texture_t t = texture_create(img.x, img.y, img.n, img.pixels, flags);
        image_destroy(&img);
        return t;
    }
    return texture_checker();
}

static array(texture_t) texture_cache = {0};

texture_t texture(const char *pathfile, int flags) {
    for (int i = 0; i < array_count(texture_cache); i++) {
        if (texture_cache[i].flags == flags && strcmp(texture_cache[i].filename, pathfile) == 0) {
            return texture_cache[i];
        }
    }
    int filesize = 0;
    void *ptr = file_read(pathfile, &filesize);
    if (ptr) {
        texture_t t = texture_from_mem(ptr, filesize, flags);
        t.filename = STRDUP(pathfile);

        if (!(flags & TEXTURE_UNIQUE)) {
            array_push(texture_cache, t);
        }
        return t;
    }
    return texture_checker();
}

void texture_destroy( texture_t *t ) {
    if (t->filename && t->filename[0]) {
        for (int i = 0; i < array_count(texture_cache); i++) {
            if (texture_cache[i].filename == t->filename) {
                array_erase_slow(texture_cache, i);
                break;
            }
        }
    }
    if(t->filename && t->filename[0]) FREE(t->filename), t->filename = 0;
    if(t->fbo) fbo_destroy_id(t->fbo), t->fbo = 0;
    if(t->id) glDeleteTextures(1, &t->id), t->id = 0;
    *t = (texture_t){0};
}


// ktx texture loader
// - rlyeh, public domain
//
// [ref] https://developer.nvidia.com/astc-texture-compression-for-game-assets
//
// # Compatibility and modes. What to choose.
// - iOS: PVRTC1_4_RGB or PVRTC1_4 (RGBA) with q:pvrtcnormal.
// - Desktop (OSX/Linux/Windows): BC1, BC1a or BC3 with q:normal.
// - Android: ETC2_RGB or ETC2_RGBA with q:etcfast. ASTC_4x4 or ASTC_8x8 with q:astcmedium, as a fallback.

#if 0
enum {
    // for glFormat
    GLFORMAT_RED                 = 0x1903,
    GLFORMAT_RG                  = 0x8227,
    GLFORMAT_RGB                 = 0x1907,
    GLFORMAT_RGBA                = 0x1908,
    //GLFORMAT_ALPHA             = 0x1906, // 8
    //GLFORMAT_LUMINANCE         = 0x1909, // 8
    //GLFORMAT_LUMINANCE_ALPHA   = 0x190A, // 88

    // for glType
    GLTYPE_UNSIGNED_BYTE         = 0x1401,

    // for glInternalFormat: RAW           // @todo: SRGB, SRGBA, SBGR, SBGRA
    UNCOMPRESSED_RGB             = 0x8051, // 888, GL_RGB8_EXT
    UNCOMPRESSED_RGB_565         = 0x8363,
    UNCOMPRESSED_RGBA            = 0x8058, // 8888, GL_RGBA8_EXT
    UNCOMPRESSED_RGBA_4444       = 0x8033,
    UNCOMPRESSED_RGBA_5551       = 0x8034,
    UNCOMPRESSED_BGR             = 0x80E0, // 888
    UNCOMPRESSED_BGRA            = 0x80E1, // 8888

    // for glInternalFormat: S3TC/DXTn/BCn // @todo: BC4,5,6,7*
    COMPRESSED_RGB_BC1           = 0x83F0, // DXT1
    COMPRESSED_RGBA_BC1          = 0x83F1, // DXT1a, BC1a
    COMPRESSED_RGBA_BC2          = 0x83F2, // DXT3
    COMPRESSED_RGBA_BC3          = 0x83F3, // DXT5
    COMPRESSED_RGBA_BC7          = 0x8E8C, // COMPRESSED_RGBA_BPTC_UNORM_ARB
    COMPRESSED_SRGB_BC1          = 0x8C4C,
    COMPRESSED_SRGBA_BC1         = 0x8C4D,
    COMPRESSED_SRGBA_BC2         = 0x8C4E,
    COMPRESSED_SRGBA_BC3         = 0x8C4F,
    // RGB_BC7f  COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB
    // RGB_BC7uf COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB
    // RGBA_BC7  COMPRESSED_RGBA_BPTC_UNORM_ARB
    // SRGBA_BC7 COMPRESSED_SRGBA_BPTC_UNORM_ARB

    // for glInternalFormat: ETC2+EAC
    COMPRESSED_R_EAC             = 0x9270, // 4bpp
    COMPRESSED_R_EAC_SIGNED      = 0x9271, // 4bpp. can preserve 0
    COMPRESSED_RG_EAC            = 0x9272, // 8bpp
    COMPRESSED_RG_EAC_SIGNED     = 0x9273, // 8bbp. can preserve 0
    COMPRESSED_RGB_ETC2          = 0x9274, // 4bpp
    COMPRESSED_RGBA_ETC2         = 0x9276, // 4bpp A1
    COMPRESSED_RGBA_ETC2_EAC     = 0x9278, // 8bpp
    COMPRESSED_SRGB_ETC2         = 0x9275, // 4bpp
    COMPRESSED_SRGBA_ETC2        = 0x9277, // 4bpp A1
    COMPRESSED_SRGBA_ETC2_EAC    = 0x9279, // 8bpp

    // for glInternalFormat: PVR
    COMPRESSED_RGB_PVR1_2        = 0x8C01,
    COMPRESSED_RGB_PVR1_4        = 0x8C00,
    COMPRESSED_RGBA_PVR1_2       = 0x8C03,
    COMPRESSED_RGBA_PVR1_4       = 0x8C02,

    COMPRESSED_SRGB_PVR1_2       = 0x8A54, // _EXT
    COMPRESSED_SRGB_PVR1_4       = 0x8A55, // _EXT
    COMPRESSED_SRGBA_PVR1_2      = 0x8A56, // _EXT
    COMPRESSED_SRGBA_PVR1_4      = 0x8A57, // _EXT

    COMPRESSED_RGBA_PVR2_2       = 0x9137,
    COMPRESSED_RGBA_PVR2_4       = 0x9138,
    COMPRESSED_SRGBA_PVR2_2      = 0x93F0,
    COMPRESSED_SRGBA_PVR2_4      = 0x93F1,

    // for glInternalFormat: ASTC
    COMPRESSED_RGBA_ASTC4x4      = 0x93B0, // 8.00bpp
    COMPRESSED_RGBA_ASTC5x4      = 0x93B1, // 6.40bpp
    COMPRESSED_RGBA_ASTC5x5      = 0x93B2, // 5.12bpp
    COMPRESSED_RGBA_ASTC6x5      = 0x93B3, // 4.27bpp
    COMPRESSED_RGBA_ASTC6x6      = 0x93B4, // 3.56bpp
    COMPRESSED_RGBA_ASTC8x5      = 0x93B5, // 3.20bpp
    COMPRESSED_RGBA_ASTC8x6      = 0x93B6, // 2.67bpp
    COMPRESSED_RGBA_ASTC8x8      = 0x93B7, // 2.56bpp
    COMPRESSED_RGBA_ASTC10x5     = 0x93B8, // 2.13bpp
    COMPRESSED_RGBA_ASTC10x6     = 0x93B9, // 2.00bpp
    COMPRESSED_RGBA_ASTC10x8     = 0x93BA, // 1.60bpp
    COMPRESSED_RGBA_ASTC10x10    = 0x93BB, // 1.28bpp
    COMPRESSED_RGBA_ASTC12x10    = 0x93BC, // 1.07bpp
    COMPRESSED_RGBA_ASTC12x12    = 0x93BD, // 0.89bpp

    COMPRESSED_SRGBA_ASTC4x4     = 0x93D0, // 8.00bpp SRGB8 A8
    COMPRESSED_SRGBA_ASTC5x4     = 0x93D1, // 6.40bpp SRGB8 A8
    COMPRESSED_SRGBA_ASTC5x5     = 0x93D2, // 5.12bpp SRGB8 A8
    COMPRESSED_SRGBA_ASTC6x5     = 0x93D3, // 4.27bpp SRGB8 A8
    COMPRESSED_SRGBA_ASTC6x6     = 0x93D4, // 3.56bpp SRGB8 A8
    COMPRESSED_SRGBA_ASTC8x5     = 0x93D5, // 3.20bpp SRGB8 A8
    COMPRESSED_SRGBA_ASTC8x6     = 0x93D6, // 2.67bpp SRGB8 A8
    COMPRESSED_SRGBA_ASTC8x8     = 0x93D7, // 2.56bpp SRGB8 A8
    COMPRESSED_SRGBA_ASTC10x5    = 0x93D8, // 2.13bpp SRGB8 A8
    COMPRESSED_SRGBA_ASTC10x6    = 0x93D9, // 2.00bpp SRGB8 A8
    COMPRESSED_SRGBA_ASTC10x8    = 0x93DA, // 1.60bpp SRGB8 A8
    COMPRESSED_SRGBA_ASTC10x10   = 0x93DB, // 1.28bpp SRGB8 A8
    COMPRESSED_SRGBA_ASTC12x10   = 0x93DC, // 1.07bpp SRGB8 A8
    COMPRESSED_SRGBA_ASTC12x12   = 0x93DD, // 0.89bpp SRGB8 A8

    // others:
    // COMPRESSED_RED_RGTC1
    // COMPRESSED_SIGNED_RED_RGTC1
    // COMPRESSED_RG_RGTC2
    // COMPRESSED_SIGNED_RG_RGTC2
};
#endif

#pragma pack(push, 1) // not really needed. the struct is exactly 64 bytes, and all members are 32-bit unsigned
typedef struct ktx_header {
    unsigned identifier[3];         // "«KTX 11»\r\n\x1A\n"
    unsigned endianness;            // 0x04030201 if match

    unsigned glType;                // 0 if compressed; otherwise: UNSIGNED_BYTE, UNSIGNED_SHORT_5_6_5, etc.
    unsigned glTypeSize;            // 1 if compressed; otherwise, size in bytes of glType for endianness conversion. not needed.
    unsigned glFormat;              // STENCIL_INDEX, DEPTH_COMPONENT, DEPTH_STENCIL, RED, GREEN, BLUE, RG, RGB, RGBA, BGR, BGRA, RED_INTEGER, GREEN_INTEGER, BLUE_INTEGER, RG_INTEGER, RGB_INTEGER, RGBA_INTEGER, BGR_INTEGER, BGRA_INTEGER,
    unsigned glInternalFormat;      // COMPRESSED_RED, COMPRESSED_RG, COMPRESSED_RGB, COMPRESSED_RGBA, COMPRESSED_SRGB, COMPRESSED_SRGB_ALPHA, COMPRESSED_RED_RGTC1, COMPRESSED_SIGNED_RED_RGTC1, COMPRESSED_RG_RGTC2, COMPRESSED_SIGNED_RG_RGTC2, COMPRESSED_RGBA_BPTC_UNORM, COMPRESSED_SRGB_ALPHA_BPTC_UNORM, COMPRESSED_RGB_BPTC_SIGNED_FLOAT, COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT, COMPRESSED_RGB8_ETC2, COMPRESSED_SRGB8_ETC2, COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, COMPRESSED_RGBA8_ETC2_EAC, COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, COMPRESSED_R11_EAC, COMPRESSED_SIGNED_R11_EAC, COMPRESSED_RG11_EAC, COMPRESSED_SIGNED_RG11_EAC,
    unsigned glBaseInternalFormat;  // DEPTH_COMPONENT, DEPTH_STENCIL, RED, RG, RGB, RGBA, STENCIL_INDEX,

    unsigned width;
    unsigned height;
    unsigned depth;

    unsigned num_surfaces; // >1 for material
    unsigned num_faces;    // =6 for cubemaps (+X,-X,+Y,-Y,+Z,-Z order), 1 otherwise
    unsigned num_mipmaps;  // >1 for mipmaps

    unsigned metadata_size; // length of following header
    // struct ktx_metadata {
    //     unsigned key_and_value_size;
    //     char     key_and_value[key_and_value_size];
    //     char     value_padding[3 - ((key_and_value_size + 3) % 4)];
    // };
    // struct ktx_texture_data {
    //     unsigned size;
    //     char     data[0];
    // } tx;
} ktx_header;
#pragma pack(pop)

typedef struct ktx_texture {
    unsigned width;
    unsigned height;
    unsigned depth;
    unsigned size;
    const char* data;
} ktx_texture;

typedef struct ktx {
    ktx_header hdr;
    const char *error;
} ktx;

static __thread array(ktx_texture) ktx_textures;

static
ktx ktx_load(const void *data, unsigned int len) {
    ktx ctx = {0};

    // check ktx signature
    bool is_ktx = (len > sizeof(ktx_header)) && !memcmp(data, "\xABKTX 11\xBB\r\n\x1A\n", 12);
    if( !is_ktx ) {
        return ctx.error = "ERROR_BAD_KTX_FILE", ctx;
    }

    // copy texture header
    ktx_header *hdr = &ctx.hdr;
    *hdr = *((const ktx_header *)data);

    // sanity checks
    STATIC_ASSERT(sizeof(ktx_header) == (16*4));
    for( int i = 0; i < sizeof(ktx_header)/4; ++i) {
        i[(unsigned*)hdr] = lil32(i[(unsigned*)hdr]);
    }
    if( hdr->endianness != 0x04030201 ) {
        return ctx.error = "ERROR_BAD_ENDIANNESS", ctx;
    }
    if( (hdr->num_faces != 1) && (hdr->num_faces != 6) ) {
        return ctx.error = "ERROR_BAD_NUMBER_OF_FACES", ctx;
    }

    // normalize glInternalFormat if uncompressed.
    if( hdr->glType != 0 ) {
        hdr->glInternalFormat = hdr->glBaseInternalFormat;
    }

    // normalize [1..N] range
    hdr->num_mipmaps += !hdr->num_mipmaps;
    hdr->num_surfaces += !hdr->num_surfaces;
    hdr->num_faces += !hdr->num_faces;

    // basically,
    // for each level in num_mipmaps { UInt32 imageSize;
    //     for each surface in num_surfaces {
    //         for each face in num_faces {
    //             for each slice in depth {
    //                 for each row in height {
    //                     for each pixel in width {
    //                         byte data[size_based_on_pixelformat]
    //         byte facePadding[0-3] }}}
    //     }
    // Byte mipPadding[0-3] }

    array_resize(ktx_textures, hdr->num_mipmaps * hdr->num_surfaces * hdr->num_faces);

    const char *bitmap = ((const char*)data) + sizeof(ktx_header) + hdr->metadata_size;

    for( unsigned m = 0; m < hdr->num_mipmaps; ++m ) {
        for( unsigned s = 0; s < hdr->num_surfaces; ++s ) {
            for( unsigned f = 0; f < hdr->num_faces; ++f ) {
                ktx_texture *t = &ktx_textures[f+s*hdr->num_faces+m*hdr->num_faces*hdr->num_surfaces];

                // set dimensions [1..N]
                t->width  = (hdr->width  >> m) + !(hdr->width  >> m);
                t->height = (hdr->height >> m) + !(hdr->height >> m);
                t->depth  = (hdr->depth  >> m) + !(hdr->depth  >> m);

                // seek to mip
                const char *ptr = bitmap;
                for( int i = 0; i <= m; i++ ) {
                    // if cubemap, *ptr holds unpadded size of single face,
                    // else, *ptr holds size of all surfaces+faces+slices for whole mipmap.
                    unsigned size = lil32(*(unsigned*)ptr);
                    unsigned padding = 3 - ((size + 3) % 4);

                    // seek to data
                    t->data = ptr + 4 + (size * f);

                    // seek to next mipmap
                    ptr = ptr + 4 + (size * hdr->num_faces) + padding;

                    // adjust size
                    t->size = (uintptr_t)(ptr - t->data); // -padding; needed?
                }

                // ensure we're in bounds
                ASSERT(t->data < ((char*)data + len), "%p < %p", t->data, ((char*)data + len));
                ASSERT(((char*)t->data+t->size) <= ((char*)data + len), "%p < %p", (char*)t->data + t->size, ((char*)data + len));
            }
        }
    }

    return ctx;
}

// ---

texture_t texture_compressed_from_mem(const void *data, int len, unsigned flags) {
    ktx ctx = ktx_load(data, len);
    if( ctx.error ) {
        // puts(ctx.error);
        // return texture_checker();
        return texture_from_mem(data, len, flags);
    }

    ktx_header hdr = ctx.hdr;

    // flags
    int target = hdr.num_faces == 6 ? GL_TEXTURE_CUBE_MAP : hdr.depth > 0 ? GL_TEXTURE_3D : GL_TEXTURE_2D;
    int dimensions = target == GL_TEXTURE_3D ? 3 : target == GL_TEXTURE_2D || target == GL_TEXTURE_CUBE_MAP ? 2 : 1;

    // create texture
    GLuint id;
    glGenTextures(1, &id);

    // apply common flags (note: we do not update pixels data at this point)
    texture_t t = {0};
    t.id = id;
    t.flags = flags;
    t.w = ktx_textures[0].width;
    t.h = ktx_textures[0].height;
    t.d = ktx_textures[0].depth;
    t.n = hdr.glFormat;
    /**/ if( t.n == GL_RG || t.n == GL_RG_INTEGER ) t.n = 2;
    else if( t.n == GL_RGB || t.n == GL_BGR || t.n == GL_RGB_INTEGER || t.n == GL_BGR_INTEGER ) t.n = 3;
    else if( t.n == GL_RGBA || t.n == GL_BGRA || t.n == GL_RGBA_INTEGER || t.n == GL_RGBA_INTEGER ) t.n = 4;
    else t.n = 1;

    texture_update(&t, t.w, t.h, t.n, NULL, flags);

    // filtering
    glBindTexture(target, id);
    // glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(target, GL_TEXTURE_MIN_FILTER, hdr.num_mipmaps > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

    // // wrapping
    // if( dimensions > 0 ) glTexParameteri(target, GL_TEXTURE_WRAP_S, flags & TEXTURE_CLAMP ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    // if( dimensions > 1 ) glTexParameteri(target, GL_TEXTURE_WRAP_T, flags & TEXTURE_CLAMP ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    // if( dimensions > 2 ) glTexParameteri(target, GL_TEXTURE_WRAP_R, flags & TEXTURE_CLAMP ? GL_CLAMP_TO_EDGE : GL_REPEAT);

    if( target == GL_TEXTURE_CUBE_MAP ) target = GL_TEXTURE_CUBE_MAP_POSITIVE_X;

    GLenum internalFormat = flags & TEXTURE_SRGB ? GL_SRGB8_ALPHA8 : hdr.glInternalFormat;

    bool checked_transparency = false;

    int bytes = 0;
    enum { border = 0 };
    if (hdr.num_mipmaps) {
        for( int m = 0; m < hdr.num_mipmaps; ++m ) {
            for( int s = 0; s < hdr.num_surfaces; ++s ) {
                for( int f = 0; f < hdr.num_faces; ++f ) {
                    int d3 = target == GL_TEXTURE_3D, compr = hdr.glType == 0, mode = d3+compr*2;
                    ktx_texture *texture = &ktx_textures[f+s*hdr.num_faces+m*hdr.num_faces*hdr.num_surfaces];
                    /**/ if(mode==0) glTexImage2D(target+f,m,internalFormat,texture->width,texture->height,                   border,hdr.glFormat,hdr.glType,texture->data);
                    else if(mode==1) glTexImage3D(target  ,m,internalFormat,texture->width,texture->height,texture->depth,          border,hdr.glFormat,hdr.glType,texture->data);
                    else if(mode==2) glCompressedTexImage2D(target+f,m,internalFormat,texture->width,texture->height,         border,texture->size,texture->data);
                    else if(mode==3) glCompressedTexImage3D(target  ,m,internalFormat,texture->width,texture->height,texture->depth,border,texture->size,texture->data);
                    bytes += texture->size;

                    if (t.n == 4 && !checked_transparency) {
                        checked_transparency = true;
                        for (int i = 0; i < texture->width * texture->height; i++) {
                            if (((uint8_t *)texture->data)[i * 4 + 3] < 255) {
                                t.transparent = 1;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

   if( hdr.num_mipmaps==1 )
   if( flags & TEXTURE_MIPMAPS ) glGenerateMipmap(target);

    PRINTF("dims:%dx%dx%d,size:%.2fMiB,mips:%d,layers:%d,faces:%d\n", t.w, t.h, t.d, bytes / 1024.0 / 1024.0, hdr.num_mipmaps, hdr.num_surfaces, hdr.num_faces);
    return t;
}

texture_t texture_compressed(const char *pathfile, unsigned flags) {
    for (int i = 0; i < array_count(texture_cache); i++) {
        if (texture_cache[i].flags == flags && strcmp(texture_cache[i].filename, pathfile) == 0) {
            return texture_cache[i];
        }
    }
    int size = 0;
    char *data = file_read(pathfile, &size);
    texture_t t = texture_compressed_from_mem(data, size, flags);
    t.filename = STRDUP(pathfile);
    if (!(flags & TEXTURE_UNIQUE)) {
        array_push(texture_cache, t);
    }
    return t;
}

#endif
