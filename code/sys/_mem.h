API void *      memset32(void *dst, unsigned ch, int words);
API const void* memmemb(const void *blob, size_t bloblen, const void * const bits, const size_t bitslen);
API const void* memstr(const void *blob, size_t bloblen, const char* str);


// find a mem blob in a mem section; similar to strstr()
const void *memfind(const void *blob, size_t bloblen, const void * const bits, const size_t bitslen) {
    if((uintptr_t)blob * bloblen * (uintptr_t)bits * bitslen)
    for (const char *h = blob; bloblen >= bitslen; ++h, --bloblen) {
        if (!memcmp(h, bits, bitslen)) {
            return h;
        }
    }
    return NULL;
}

const void *memstr(const void *blob, size_t bloblen, const char* str) {
    return memfind(blob, bloblen, str, strlen(str));
}

// memset words instead of chars
void *memset32(void *dst, unsigned ch, int words) {
    unsigned *ptr = (unsigned*)dst;
    if( words > 0 )
    while( words-- ) {
        *ptr++ = ch;
    }
    return dst;
}


// -----------------------------------------------------------------------------
// memory framework
// - rlyeh, public domain

#if !CODE

// memory leaks detector
#if ENABLE_MEMORY_LEAKS
#define WATCH(ptr,sz) watch((ptr), (sz))
#define FORGET(ptr)   forget(ptr)
#else
#define WATCH(ptr,sz) (ptr)
#define FORGET(ptr)   (ptr)
#endif

// default allocator (aborts on out-of-mem)
API void*  xrealloc(void* p, size_t sz);
API size_t xsize(void* p);
API char*  xstats(void);

// stack based allocator (negative bytes does rewind stack, like when entering new frame)
API void*  stack(int bytes);

// memory leaks api (this is already integrated as long as you compile with -DENABLE_MEMORY_LEAKS)
API void*  watch( void *ptr, int sz );
API void*  forget( void *ptr );

// memory api
#define ALLOCSIZE(p)   xsize(p)
#define MALLOC(n)      REALLOC_(0,(n))
#define FREE(p)        REALLOC_((p), 0)
#define REALLOC(p,n)   REALLOC_((p),(n))
#define CALLOC(m,n)    CALLOC_((m),(n))
#define STRDUP(s)      STRDUP_(s)
#define ALLOCA(n)      ifdef(gcc, __builtin_alloca(n), ifdef(win32, _alloca(n), __builtin_alloca(n)))

static FORCE_INLINE void *(REALLOC_)(void *p, size_t n) { return n ? WATCH(xrealloc(p,n),n) : xrealloc(FORGET(p),0); } ///-
static FORCE_INLINE void *(CALLOC_)(size_t m, size_t n) { return n *= m, memset(REALLOC(0,n),0,n); } ///-
static FORCE_INLINE char *(STRDUP_)(const char *s) { size_t n = strlen(s)+1; return ((char*)memcpy(REALLOC(0,n), s, n)); } ///-

#else

size_t dlmalloc_usable_size(void*); // __ANDROID_API__

#if is(bsd) || is(osx) // bsd or osx
#  include <malloc/malloc.h>
#else
#  include <malloc.h>
#endif

#ifndef SYS_MEM_INIT
#define SYS_MEM_INIT()
#define SYS_MEM_REALLOC realloc
#define SYS_MEM_SIZE /* bsd/osx, then win32, then ems/__GLIBC__, then __ANDROID_API__ */ \
    ifdef(osx, malloc_size, ifdef(bsd, malloc_size, \
        ifdef(win32, _msize, malloc_usable_size)))
#endif

// xrealloc --------------------------------------------------------------------

static __thread uint64_t xstats_current = 0, xstats_total = 0, xstats_allocs = 0;

void* xrealloc(void* oldptr, size_t size) {
    static __thread int once = 0; for(;!once;once = 1) SYS_MEM_INIT();

    // for stats
    size_t oldsize = xsize(oldptr);

    void *ptr = SYS_MEM_REALLOC(oldptr, size);
    if( !ptr && size ) {
        die("Not memory enough (trying to allocate %u bytes)", (unsigned)size);
    }
#if ENABLE_MEMORY_POISON
    if( !oldptr && size ) {
        memset(ptr, 0xCD, size);
    }
#endif

    // for stats
    if( oldptr ) {
        xstats_current += (int64_t)size - (int64_t)oldsize;
        xstats_allocs -= !size;
    } else {
        xstats_current += size;
        xstats_allocs += !!size;
    }
    if( xstats_current > xstats_total ) {
        xstats_total = xstats_current;
    }

    return ptr;
}
size_t xsize(void* p) {
    if( p ) return SYS_MEM_SIZE(p);
    return 0;
}
char *xstats(void) {
    uint64_t xtra = 0; // xstats_allocs * 65536; // assumes 64K pagesize for every alloc
    return va("%03u/%03uMB", (unsigned)((xstats_current+xtra) / 1024 / 1024), (unsigned)((xstats_total+xtra) / 1024 / 1024));
}

// stack -----------------------------------------------------------------------

void* stack(int bytes) { // use negative bytes to rewind stack
    static __thread uint8_t *stack_mem = 0;
    static __thread uint64_t stack_ptr = 0;
    static __thread uint64_t stack_max = 0; // watch this var, in case you want to fine tune 4 MiB value below
    if( bytes < 0 ) {
        if( stack_ptr > stack_max ) stack_max = stack_ptr;
        return (stack_ptr = 0), NULL;
    }
    if( !stack_mem ) stack_mem = xrealloc(stack_mem, xsize(stack_mem) + 4 * 1024 * 1024);
    return &stack_mem[ (stack_ptr += bytes) - bytes ];
}

// leaks ----------------------------------------------------------------------

void* watch( void *ptr, int sz ) {
    static __thread int open = 1;
    if( ptr && open ) {
        open = 0;

        char buf[256];
        sprintf(buf, "%p.mem", ptr);
        for( FILE *fp = fopen(buf, "a+"); fp; fclose(fp), fp = 0 ) {
            fseek(fp, 0L, SEEK_END);
            const char *cs = callstack( +16 ); // +48
            fprintf(fp, "Built %s %s\n", __DATE__, __TIME__); // today() instead?
            fprintf(fp, "Memleak address: [%p], size: %d\n%s\n", ptr, sz, cs ? cs : "No callstack.");
        }

        open = 1;
    }
    return ptr;
}
void* forget( void *ptr ) {
    if( ptr ) {
        char buf[256];
        sprintf(buf, "%p.mem", ptr);
        unlink(buf);
    }
    return ptr;
}

#endif

