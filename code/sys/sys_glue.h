#if !CODE

API void fn_void();
API void fn_void_int(int);
API void fn_void_str(const char *);

#ifdef _MSC_VER
#define __attribute__(...) __attr1bute__ __VA_ARGS__
#define __attr1bute__(...) __attribute__##__VA_ARGS__
#define __attribute__malloc
#define __attribute__alloc_size(...)
#define __attribute__format(...)
#endif

//-----------------------------------------------------------------------------
// compat (unix & stdio.h)

#if is(win32)
#include <io.h>
#include <direct.h>
#if is(mingw)
#include <unistd.h>
#endif
#else
#include <unistd.h>
#include <sched.h> // sched_setaffinity(), CPU_ZERO(), CPU_COUNT()
#include <sys/ioctl.h>
#endif

#if is(ems)
//#define unlink(x) ((void)(x), 0)
#endif

#if is(win32)
//#define alloca        _alloca
#define atoi64          _atoi64
#define popen           _popen
#define pclose          _pclose
//#define strncasecmp   _strnicmp
#define mkdir(  p,m)    mkdir(p)
#define chdir           ifdef(cl, _chdir, chdir)
#if is(cl) || is(tcc)
#define ftruncate       _chsize_s
#endif
#define flockfile       ifdef(cl,_lock_file,ifdef(mingw,_lock_file,(void)))
#define funlockfile     ifdef(cl,_unlock_file,ifdef(mingw,_unlock_file,(void)))
#else // gcc
//#include <alloca.h> // mingw64 does not have it
#include <strings.h>  // strncasecmp
#define atoi64          atoll
//#define strstri       strcasestr
//#define strcmpi       strcasecmp
#endif



#if 0
#if !is(cl)
#define tmpfile file_temp
#endif

#define tmpnam(x) file_tempname()
#endif

#if 0
static
const char *pathfile_from_handle(FILE *fp) {
#if is(win32)
    int fd = fileno(fp);
    HANDLE handle = (HANDLE)_get_osfhandle( fd ); // <io.h>
    DWORD size = GetFinalPathNameByHandleW(handle, NULL, 0, VOLUME_NAME_DOS);
    wchar_t name[DIR_MAX] = L"";
    size = GetFinalPathNameByHandleW(handle, name, size, VOLUME_NAME_DOS);
    name[size] = L'\0';
    return wchar16to8(name + 4); // skip \\?\ header
#else
    // In Linux, you can use readlink on /proc/self/fd/NNN where NNN is the file descriptor
    // In OSX:
    //     #include <sys/syslimits.h>
    //     #include <fcntl.h>
    //     char filePath[DIR_MAX];
    //     if (fcntl(fd, F_GETPATH, filePath) != -1) {
    //         // do something with the file path
    //     }
    return 0;
#endif
}
#endif


#if defined __TINYC__ && defined _WIN32
#define MAPVK_VSC_TO_VK 1
#define MAPVK_VK_TO_VSC 0
#define IPV6_V6ONLY 27
#define _WIN32_WINNT_VISTA 0
#define _WIN32_WINNT_WINXP 0
#define _WIN32_WINNT_WIN7 0
#endif
#ifdef __TINYC__
#define STBI_NO_SIMD
// no uint128_t (3rd_https.h)
char* strtok_s(
   char* str,
   const char* delimiters,
   char** context
);
#endif

#if defined __clang__ && defined _WIN32
int execv(const char *path, char *const argv[]);
#elif (is(tcc) /*|| defined __clang__*/) && defined _WIN32
int execv(const char *path, char *const argv[]);
errno_t strerror_s(
   char *buffer,
   size_t sizeInBytes,
   int errnum
);
typedef int socklen_t;
#if is(tcc)
#define restrict
const char *inet_ntop(int af, const void *restrict src,
                     char *restrict dst, socklen_t size);
int inet_pton(int af, const char *restrict src, void *restrict dst);
#endif
errno_t fopen_s(
   FILE** pFile,
   const char *filename,
   const char *mode
);
#endif

//#pragma GCC warning "warning message"         // gcc/clang only pragma
//#pragma message ( "your warning text here" )  // msc pragma which works in gcc too, albeit not as intended

#ifdef _MSC_VER
#define bswap16  _byteswap_ushort
#define bswap64  _byteswap_uint64
#define __thread __declspec(thread)
#else
#define bswap16 __builtin_bswap16
#define bswap64 __builtin_bswap64
#endif

#ifdef _WIN32
#define mkdir(p,m)    mkdir(p)
#define popen _popen
#define pclose _pclose
#else
#include <limits.h>
#define MAX_PATH PATH_MAX // (defined in limits.h)

#define strcmpi       strcasecmp
#ifdef __GNUC__ // also, clang
    int __argc;
    char **__argv;
    __attribute__((constructor)) void init_argcv(int argc, char **argv) {
        __argc = argc;
        __argv = argv;
    }
#endif
#endif

#ifdef _MSC_VER
#include <immintrin.h>
#define __builtin_clz(x)   _lzcnt_u32(x)
#define __builtin_clzll(x) _lzcnt_u64(x)
#define __builtin_ctz(x)   _tzcnt_u32(x)
#define __builtin_ctzll(x) _tzcnt_u64(x)
#endif

// ----------------------------------------------------------------------------
// non-gnu fixes

#if is(msc)
#define __thread                   __declspec(thread)
#elif is(tcc) && is(win32)
#define __thread                   __declspec(thread) // compiles fine apparently, but does not work
#elif is(tcc)
#define __thread
#endif

// -----------------------------------------------------------------------------
// tcc fixes

#if is(win32) && is(tcc) && is(x64)
// missing libm symbols on tinycc HEAD repo (tcc-x64 pre-0.9.28)
#include <tgmath.h>
#define sqrtf sqrt
#define sinf sin
#define asinf asin
#define cosf cos
#define acosf acos
#define tanf tan
#define atan2f atan2
#define powf pow
#define floorf floor
#define logf log
#define ceilf ceil
#define copysignf copysign
#define expf exp
#define fmodf fmod
#define log10f log10
#define hypotf hypot
// `tcc0.9.27 -m64` workaround (win) for all functions with ending bool argument
#undef  bool
typedef char bool; ///-
#endif

#if defined __TINYC__ && defined __linux
#   ifndef __builtin_alloca
#   define __builtin_alloca alloca
#   endif
#endif


#else // CODE

#if is(tcc) && is(win32) && defined ENET_H // add missing symbols
const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;        /* :: */
const struct in6_addr in6addr_loopback;   /* ::1 */
#endif

#if is(win32) // _MSC_VER and __MINGW64__
#include <stdio.h>
#include <windows.h>
#include <share.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
FILE *fmemopen(void *buf, size_t len, const char *type) {
    int fd = -1;
    char temppath[DIR_MAX - 14], filename[DIR_MAX + 1];
    if( GetTempPathA(sizeof(temppath), temppath) )
    if( GetTempFileNameA(temppath, ".tempfile", 0, filename) )
    if( !_sopen_s(&fd, filename, _O_CREAT | _O_SHORT_LIVED | _O_TEMPORARY | _O_RDWR | _O_BINARY | _O_NOINHERIT, _SH_DENYRW, _S_IREAD | _S_IWRITE) )
    for( FILE *fp = fd != -1 ? _fdopen(fd, "w+b") : NULL; fp; )
    return fwrite(buf, len, 1, fp), rewind(fp), unlink(filename), fp; // no need to _close. fclose(on the returned FILE*) also _closes the file descriptor.
    return fd != -1 ? _close(fd), NULL : NULL;
}
#endif

// Enable more performant GPUs on laptops. Does this work into a dll?
// int NvOptimusEnablement = 1;
// int AmdPowerXpressRequestHighPerformance = 1;

#if is(linux) && is(tcc) // fixes `tcc: error: undefined symbol '__dso_handle'`
int __dso_handle; // compiled with: `tcc demo.c engine.c -D__STDC_NO_VLA__ -lX11`
#endif

#if is(win32) && is(tcc) // fixes `tcc: error: undefined symbol '_InterlockedExchangeAdd'` when compiling with `-m64` flag
__CRT_INLINE LONG _InterlockedExchangeAdd(LONG volatile *add, LONG val) {
    LONG old;
    do old = *add; while( InterlockedCompareExchange(add, old + val, old) != old );
    return old;
}
__CRT_INLINE LONGLONG _InterlockedExchangeAdd64(LONGLONG volatile *add, LONGLONG val) { // 64bit version, for completeness
    LONGLONG old;
    do old = *add; while( InterlockedCompareExchange64(add, old + val, old) != old );
    return old;
}
#endif

#ifdef ZIG_CC
static int IN6_IS_ADDR_V4MAPPED(const struct in6_addr *a) { return ((a->s6_words[0]==0) && (a->s6_words[1]==0) && (a->s6_words[2]==0) && (a->s6_words[3]==0) && (a->s6_words[4]==0) && (a->s6_words[5]==0xffff)); }
const struct in6_addr in6addr_any; // = IN6ADDR_ANY_INIT;
//static const struct in6_addr in6addr_loopback = IN6ADDR_LOOPBACK_INIT;
#endif

void fn_void() {}
void fn_void_int(int arg) {}
void fn_void_str(const char *arg) {}

#endif // CODE
