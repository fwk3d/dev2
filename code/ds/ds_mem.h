#if !CODE

size_t dlmalloc_usable_size(void*); // __ANDROID_API__

#if is(bsd) || is(osx) // bsd or osx
#  include <malloc/malloc.h>
#else
#  include <malloc.h>
#endif

#ifndef SYS_MEM_REALLOC
#define SYS_MEM_REALLOC (realloc)
#define SYS_MEM_SIZE /* bsd/osx, then win32, then ems/__GLIBC__, then __ANDROID_API__ */ \
    ifdef(osx, (malloc_size), ifdef(bsd, (malloc_size), \
        ifdef(win32, (_msize), (malloc_usable_size))))
#endif

// ----------------------------------------------------------------------------

// 0,n  alloc
// p,n  realloc
// p,0  free
// p,~0 returns pointer size
// 0,~0 returns global mem stats (char*)

#include <stddef.h>
typedef void* (*allocator_t)(void*, size_t);

// ----------------------------------------------------------------------------

//API extern allocator_t xrealloc;
API void* xrealloc(void*, size_t);

#if ENABLE_MEMORY_LEAKS
#define WATCH(p,sz) watch((p),(sz))
#define FORGET(p)   forget(p)
#else
#define WATCH(p,sz) (p)
#define FORGET(p)   (p)
#endif

// memory api
#define ALLOCSIZE(p)   ((int)REALLOC_((p),~0)) // xsize(p)
#define MALLOC(n)      REALLOC_(0,(n))
#define FREE(p)        REALLOC_((p), 0)
#define REALLOC(p,n)   REALLOC_((p),(n))
#define CALLOC(m,n)    CALLOC_((m),(n))
#define STRDUP(s)      STRDUP_(s)
#define ALLOCA(n)      ifdef(gcc, __builtin_alloca(n), ifdef(win32, _alloca(n), __builtin_alloca(n)))

static FORCE_INLINE void *(REALLOC_)(void *p, size_t n) { return n ? WATCH(xrealloc(p,n),n) : xrealloc(FORGET(p),0); } ///-
static FORCE_INLINE void *(CALLOC_)(size_t m, size_t n) { return n *= m, memset(REALLOC(0,n),0,n); } ///-
static FORCE_INLINE char *(STRDUP_)(const char *s) { size_t n = strlen(s)+1; return ((char*)memcpy(REALLOC(0,n), s, n)); } ///-

// stack -----------------------------------------------------------------------

#define stack(n) va("%*.s",(n)-1,"")

#else

// xrealloc --------------------------------------------------------------------

static __thread uint64_t xstats_current = 0, xstats_total = 0, xstats_allocs = 0;

char *xstats(void) {
    static __thread char buf[32];
    uint64_t xtra = 0; // xstats_allocs * 65536; // assumes 64K pagesize for every alloc
    snprintf(buf, 32, "%03u/%03uMB", (unsigned)((xstats_current+xtra) / 1024 / 1024), (unsigned)((xstats_total+xtra) / 1024 / 1024));
    return buf;
}
size_t xsize(void* p) {
    if( p ) return SYS_MEM_SIZE(p);
    return 0;
}
void* xrealloc(void* oldptr, size_t size) {
    if( size == ~0  ) {
        return oldptr ? (void*)(uintptr_t)SYS_MEM_SIZE(oldptr) : (void*)xstats();
    }

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

#endif
