// ----------------------------------------------------------------------------
// compression api

#if !CODE

enum COMPRESS_FLAGS {
    COMPRESS_RAW     = 0,
    COMPRESS_PPP     = (1<<4),
    COMPRESS_ULZ     = (2<<4),
    COMPRESS_LZ4     = (3<<4),
    COMPRESS_CRUSH   = (4<<4),
    COMPRESS_DEFLATE = (5<<4),
    COMPRESS_LZP1    = (6<<4),
    COMPRESS_LZMA    = (7<<4),
    COMPRESS_BALZ    = (8<<4),
    COMPRESS_LZW3    = (9<<4),
    COMPRESS_LZSS    = (10<<4),
    COMPRESS_BCM     = (11<<4),
    COMPRESS_ZLIB    = (12<<4), // same as deflate with header
};

API unsigned zbounds(unsigned inlen, unsigned flags);
API unsigned zencode(void *out, unsigned outlen, const void *in, unsigned inlen, unsigned flags);
API unsigned zexcess(unsigned flags);
API unsigned zdecode(void *out, unsigned outlen, const void *in, unsigned inlen, unsigned flags);

#else

// ----------------------------------------------------------------------------
// compression api

static struct zcompressor {
    // id of compressor
    unsigned enumerator;
    // name of compressor
    const char name1, *name4, *name;
    // returns worst case compression estimation for selected flags
    unsigned (*bounds)(unsigned bytes, unsigned flags);
    // returns number of bytes written. 0 if error.
    unsigned (*encode)(const void *in, unsigned inlen, void *out, unsigned outcap, unsigned flags);
    // returns number of excess bytes that will be overwritten when decoding.
    unsigned (*excess)(unsigned flags);
    // returns number of bytes written. 0 if error.
    unsigned (*decode)(const void *in, unsigned inlen, void *out, unsigned outcap);
} zlist[] = {
    { COMPRESS_RAW,     '0', "raw",  "raw",     raw_bounds, raw_encode, raw_excess, raw_decode },
    { COMPRESS_PPP,     'p', "ppp",  "ppp",     ppp_bounds, ppp_encode, ppp_excess, ppp_decode },
    { COMPRESS_ULZ,     'u', "ulz",  "ulz",     ulz_bounds, ulz_encode, ulz_excess, ulz_decode },
    { COMPRESS_LZ4,     '4', "lz4x", "lz4x",    lz4x_bounds, lz4x_encode, lz4x_excess, lz4x_decode },
    { COMPRESS_CRUSH,   'c', "crsh", "crush",   crush_bounds, crush_encode, crush_excess, crush_decode },
    { COMPRESS_DEFLATE, 'd', "defl", "deflate", deflate_bounds, deflate_encode, deflate_excess, deflate_decode },
    { COMPRESS_LZP1,    '1', "lzp1", "lzp1",    lzp1_bounds, lzp1_encode, lzp1_excess, lzp1_decode },
    { COMPRESS_LZMA,    'm', "lzma", "lzma",    lzma_bounds, lzma_encode, lzma_excess, lzma_decode },
    { COMPRESS_BALZ,    'b', "balz", "balz",    balz_bounds, balz_encode, balz_excess, balz_decode },
    { COMPRESS_LZW3,    'w', "lzw3", "lzrw3a",  lzrw3a_bounds, lzrw3a_encode, lzrw3a_excess, lzrw3a_decode },
    { COMPRESS_LZSS,    's', "lzss", "lzss",    lzss_bounds, lzss_encode, lzss_excess, lzss_decode },
    { COMPRESS_BCM,     'B', "bcm",  "bcm",     bcm_bounds, bcm_encode, bcm_excess, bcm_decode },
    { COMPRESS_ZLIB,    'z', "zlib", "zlib",    deflate_bounds, deflatez_encode, deflate_excess, deflatez_decode },
};

enum { COMPRESS_NUM = 14 };

static char *znameof(unsigned flags) {
    static __thread char buf[16];
    snprintf(buf, 16, "%4s.%c", zlist[(flags>>4)&0x0F].name4, "0123456789ABCDEF"[flags&0xF]);
    return buf;
}
unsigned zencode(void *out, unsigned outlen, const void *in, unsigned inlen, unsigned flags) {
    return zlist[(flags >> 4) % COMPRESS_NUM].encode(in, inlen, (uint8_t*)out, outlen, flags & 0x0F);
}
unsigned zdecode(void *out, unsigned outlen, const void *in, unsigned inlen, unsigned flags) {
    return zlist[(flags >> 4) % COMPRESS_NUM].decode((uint8_t*)in, inlen, out, outlen);
}
unsigned zbounds(unsigned inlen, unsigned flags) {
    return zlist[(flags >> 4) % COMPRESS_NUM].bounds(inlen, flags & 0x0F);
}
unsigned zexcess(unsigned flags) {
    return zlist[(flags >> 4) % COMPRESS_NUM].excess(flags & 0x0F);
}

#endif
