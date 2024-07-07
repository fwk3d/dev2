// endian ---------------------------------------------------------------------
// - rlyeh, public domain

#if !CODE

API int         is_big();
API int         is_little();

API uint16_t    swap16( uint16_t x );
API uint32_t    swap32( uint32_t x );
API uint64_t    swap64( uint64_t x );
API float       swap32f(float n);
API double      swap64f(double n);
API void        swapf(float *a, float *b);
API void        swapf2(float a[2], float b[2]);
API void        swapf3(float a[3], float b[3]);
API void        swapf4(float a[4], float b[4]);

API uint16_t    lil16(uint16_t n); // swap16 as lil
API uint32_t    lil32(uint32_t n); // swap32 as lil
API uint64_t    lil64(uint64_t n); // swap64 as lil
API float       lil32f(float n);   // swap32 as lil
API double      lil64f(double n);  // swap64 as lil

API uint16_t    big16(uint16_t n); // swap16 as big
API uint32_t    big32(uint32_t n); // swap32 as big
API uint64_t    big64(uint64_t n); // swap64 as big
API float       big32f(float n);   // swap32 as big
API double      big64f(double n);  // swap64 as big

API uint16_t*   lil16p(void *p, int sz);
API uint32_t*   lil32p(void *p, int sz);
API uint64_t*   lil64p(void *p, int sz);
API float   *   lil32pf(void *p, int sz);
API double  *   lil64pf(void *p, int sz);

API uint16_t*   big16p(void *p, int sz);
API uint32_t*   big32p(void *p, int sz);
API uint64_t*   big64p(void *p, int sz);
API float   *   big32pf(void *p, int sz);
API double  *   big64pf(void *p, int sz);

#if is(cl)
#define swap16 _byteswap_ushort
#define swap32 _byteswap_ulong
#define swap64 _byteswap_uint64
#elif is(gcc)
#define swap16 __builtin_bswap16
#define swap32 __builtin_bswap32
#define swap64 __builtin_bswap64
#endif

#define hton16 big16
#define ntoh16 big16
#define hton32 big32
#define ntoh32 big32
#define hton64 big64
#define ntoh64 big64

#define IS_BIG    ((*(uint16_t *)"\0\1") == 1)
#define IS_LITTLE ((*(uint16_t *)"\0\1") != 1)

#else

int is_big() { return IS_BIG; }
int is_little() { return IS_LITTLE; }

uint16_t  lil16(uint16_t n) { return IS_BIG     ? swap16(n) : n; }
uint32_t  lil32(uint32_t n) { return IS_BIG     ? swap32(n) : n; }
uint64_t  lil64(uint64_t n) { return IS_BIG     ? swap64(n) : n; }
uint16_t  big16(uint16_t n) { return IS_LITTLE  ? swap16(n) : n; }
uint32_t  big32(uint32_t n) { return IS_LITTLE  ? swap32(n) : n; }
uint64_t  big64(uint64_t n) { return IS_LITTLE  ? swap64(n) : n; }

float     lil32f(float n)  { return IS_BIG     ? swap32f(n) : n; }
double    lil64f(double n) { return IS_BIG     ? swap64f(n) : n; }
float     big32f(float n)  { return IS_LITTLE  ? swap32f(n) : n; }
double    big64f(double n) { return IS_LITTLE  ? swap64f(n) : n; }

uint16_t* lil16p(void *p, int sz)  { if(IS_BIG    ) { uint16_t *n = (uint16_t *)p; for(int i = 0; i < sz; ++i) n[i] = swap16(n[i]); } return p; }
uint16_t* big16p(void *p, int sz)  { if(IS_LITTLE ) { uint16_t *n = (uint16_t *)p; for(int i = 0; i < sz; ++i) n[i] = swap16(n[i]); } return p; }
uint32_t* lil32p(void *p, int sz)  { if(IS_BIG    ) { uint32_t *n = (uint32_t *)p; for(int i = 0; i < sz; ++i) n[i] = swap32(n[i]); } return p; }
uint32_t* big32p(void *p, int sz)  { if(IS_LITTLE ) { uint32_t *n = (uint32_t *)p; for(int i = 0; i < sz; ++i) n[i] = swap32(n[i]); } return p; }
uint64_t* lil64p(void *p, int sz)  { if(IS_BIG    ) { uint64_t *n = (uint64_t *)p; for(int i = 0; i < sz; ++i) n[i] = swap64(n[i]); } return p; }
uint64_t* big64p(void *p, int sz)  { if(IS_LITTLE ) { uint64_t *n = (uint64_t *)p; for(int i = 0; i < sz; ++i) n[i] = swap64(n[i]); } return p; }

float   * lil32pf(void *p, int sz) { if(IS_BIG    ) { float    *n = (float    *)p; for(int i = 0; i < sz; ++i) n[i] = swap32f(n[i]); } return p; }
float   * big32pf(void *p, int sz) { if(IS_LITTLE ) { float    *n = (float    *)p; for(int i = 0; i < sz; ++i) n[i] = swap32f(n[i]); } return p; }
double  * lil64pf(void *p, int sz) { if(IS_BIG    ) { double   *n = (double   *)p; for(int i = 0; i < sz; ++i) n[i] = swap64f(n[i]); } return p; }
double  * big64pf(void *p, int sz) { if(IS_LITTLE ) { double   *n = (double   *)p; for(int i = 0; i < sz; ++i) n[i] = swap64f(n[i]); } return p; }

#if !is(cl) && !is(gcc)
uint16_t (swap16)( uint16_t x ) { return (x << 8) | (x >> 8); }
uint32_t (swap32)( uint32_t x ) { x = ((x << 8) & 0xff00ff00) | ((x >> 8) & 0x00ff00ff); return (x << 16) | (x >> 16); }
uint64_t (swap64)( uint64_t x ) { x = ((x <<  8) & 0xff00ff00ff00ff00ULL) | ((x >>  8) & 0x00ff00ff00ff00ffULL); x = ((x << 16) & 0xffff0000ffff0000ULL) | ((x >> 16) & 0x0000ffff0000ffffULL); return (x << 32) | (x >> 32); }
#endif

float    swap32f(float n)  { union { float  t; uint32_t i; } conv; conv.t = n; conv.i = swap32(conv.i); return conv.t; }
double   swap64f(double n) { union { double t; uint64_t i; } conv; conv.t = n; conv.i = swap64(conv.i); return conv.t; }

void swapf(float *a, float *b) {
    float t = *a; *a = *b; *b = *a;
}
void swapf2(float a[2], float b[2]) {
    float x = a[0]; a[0] = b[0]; b[0] = a[0];
    float y = a[1]; a[1] = b[1]; b[1] = a[1];
}
void swapf3(float a[3], float b[3]) {
    float x = a[0]; a[0] = b[0]; b[0] = a[0];
    float y = a[1]; a[1] = b[1]; b[1] = a[1];
    float z = a[2]; a[2] = b[2]; b[2] = a[2];
}
void swapf4(float a[4], float b[4]) {
    float x = a[0]; a[0] = b[0]; b[0] = a[0];
    float y = a[1]; a[1] = b[1]; b[1] = a[1];
    float z = a[2]; a[2] = b[2]; b[2] = a[2];
    float w = a[3]; a[3] = b[3]; b[3] = a[3];
}

#endif
