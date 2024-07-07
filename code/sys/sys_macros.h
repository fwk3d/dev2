#ifndef is

// aliases @fixme
#define DEV 0
#define ifdef_cl  ifdef_msc
#define ifndef_cl ifndef_msc
#define ifdef_DEBUG ifdef_debug
#define ifdef_RETAIL ifdef_retail
#define do_once ONCE
#define countof COUNTOF
#if HAS_COOK
#define ifdef_cook ifdef_1
#define ifdef_nocook ifdef_0
#else
#define ifdef_cook ifdef_0
#define ifdef_nocook ifdef_1
#endif
#ifndef HAS_BITFIELDS
#define HAS_BITFIELDS 1
#endif
#if HAS_BITFIELDS
#define ifdef_HAS_BITFIELDS ifdef_1
#else
#define ifdef_HAS_BITFIELDS ifdef_0
#endif

// ----------------------------------------------------------------------------
// if/n/def hell

#define is(macro)                      ifdef_##macro(1,0)
#define ifdef(macro, yes, /*no*/...)   ifdef_##macro(yes, __VA_ARGS__)
#define ifndef(macro, yes, /*no*/...)  ifdef_##macro(__VA_ARGS__, yes)
#define ifdef_1(yes, /*no*/...)        yes
#define ifdef_0(yes, /*no*/...)        __VA_ARGS__

#ifdef __cplusplus
#define ifdef_c                        ifdef_0
#define ifdef_cpp                      ifdef_1
#else
#define ifdef_c                        ifdef_1
#define ifdef_cpp                      ifdef_0
#endif

#ifdef _MSC_VER
#define ifdef_gcc                      ifdef_0
#define ifdef_mingw                    ifdef_0
#define ifdef_tcc                      ifdef_0
#define ifdef_msc                      ifdef_1
#elif defined __TINYC__
#define ifdef_gcc                      ifdef_0
#define ifdef_mingw                    ifdef_0
#define ifdef_tcc                      ifdef_1
#define ifdef_msc                      ifdef_0
#elif defined __MINGW64__ || defined __MINGW32__
#define ifdef_gcc                      ifdef_1
#define ifdef_mingw                    ifdef_1
#define ifdef_tcc                      ifdef_0
#define ifdef_msc                      ifdef_0
#else // also __clang__
#define ifdef_gcc                      ifdef_1
#define ifdef_mingw                    ifdef_0
#define ifdef_tcc                      ifdef_0
#define ifdef_msc                      ifdef_0
#endif

#if defined _WIN32
#define ifdef_win32                    ifdef_1
#define ifdef_linux                    ifdef_0
#define ifdef_osx                      ifdef_0
#define ifdef_bsd                      ifdef_0
#define ifdef_ems                      ifdef_0
#elif defined __linux__ || defined __linux
#define ifdef_win32                    ifdef_0
#define ifdef_linux                    ifdef_1
#define ifdef_osx                      ifdef_0
#define ifdef_bsd                      ifdef_0
#define ifdef_ems                      ifdef_0
#elif defined __APPLE__
#define ifdef_win32                    ifdef_0
#define ifdef_linux                    ifdef_0
#define ifdef_osx                      ifdef_1
#define ifdef_bsd                      ifdef_0
#define ifdef_ems                      ifdef_0
#elif defined __EMSCRIPTEN__
#define ifdef_win32                    ifdef_0
#define ifdef_linux                    ifdef_0
#define ifdef_osx                      ifdef_0
#define ifdef_bsd                      ifdef_0
#define ifdef_ems                      ifdef_1
#else // __FreeBSD__ || @todo: __ANDROID_API__
#define ifdef_win32                    ifdef_0
#define ifdef_linux                    ifdef_0
#define ifdef_osx                      ifdef_0
#define ifdef_bsd                      ifdef_1
#define ifdef_ems                      ifdef_0
#endif

// rely on NDEBUG as the official/portable way to disable asserts
// we extend NDEBUG=[0,1,2,3] to signal the compiler optimization flags O0,O1,O2,O3
#if   defined NDEBUG && NDEBUG >= 3
#define ifdef_O3                       ifdef_1
#define ifdef_O2                       ifdef_0
#define ifdef_O1                       ifdef_0
#define ifdef_O0                       ifdef_0
#define ifdef_retail                   ifdef_1
#define ifdef_release                  ifdef_0
#define ifdef_debug                    ifdef_0
#elif defined NDEBUG && NDEBUG >= 2
#define ifdef_O3                       ifdef_0
#define ifdef_O2                       ifdef_1
#define ifdef_O1                       ifdef_0
#define ifdef_O0                       ifdef_0
#define ifdef_retail                   ifdef_0
#define ifdef_release                  ifdef_1
#define ifdef_debug                    ifdef_0
#elif defined NDEBUG && NDEBUG >= 1
#define ifdef_O3                       ifdef_0
#define ifdef_O2                       ifdef_0
#define ifdef_O1                       ifdef_1
#define ifdef_O0                       ifdef_0
#define ifdef_retail                   ifdef_0
#define ifdef_release                  ifdef_1
#define ifdef_debug                    ifdef_0
#else
#define ifdef_O3                       ifdef_0
#define ifdef_O2                       ifdef_0
#define ifdef_O1                       ifdef_0
#define ifdef_O0                       ifdef_1
#define ifdef_retail                   ifdef_0
#define ifdef_release                  ifdef_0
#define ifdef_debug                    ifdef_1
#endif

#if (defined INTPTR_MAX && INTPTR_MAX == INT64_MAX) || defined(_M_X64) || defined(__amd64__) || defined(__x86_64__) || defined(__x86_64) || defined(__ppc64__) || __WORDSIZE == 64
#define ifdef_x64                      ifdef_1
#define ifdef_x32                      ifdef_0
#else
#define ifdef_x64                      ifdef_0
#define ifdef_x32                      ifdef_1
#endif

// ----------------------------------------------------------------------------
// other new macros

#ifndef ASSERT
#define ASSERT(expr, ...)          ifdef(RETAIL, (void)0, do { int fool_msvc[] = {0,}; if(!(expr)) { fool_msvc[0]++;      alert(va("!Expression failed: " #expr " " FILELINE "\n" __VA_ARGS__)), /*breakpoint*/abort(); } } while(0) )
#endif

#ifndef ASSERT_ONCE
#define ASSERT_ONCE(expr, ...)     ifdef(RETAIL, (void)0, do { int fool_msvc[] = {0,}; if(!(expr)) { fool_msvc[0]++; ONCE alert(va("!Expression failed: " #expr " " FILELINE "\n" __VA_ARGS__)), /*breakpoint*/abort(); } } while(0) )
#endif

#ifndef AUTORUN
#define AUTORUN AUTORUN_( UNIQUE(fn) )
#ifdef __cplusplus
#define AUTORUN_(fn) \
    static void fn(void); \
    static const int CONCAT(fn,__1) = (fn(), 1); \
    static void fn(void)
#elif defined _MSC_VER && !defined(__clang__) // cl, but not clang-cl
#define AUTORUN_(fn) \
    static void fn(void); \
    static int CONCAT(fn,__1) (){ fn(); return 0; } \
    __pragma(section(".CRT$XIU", long, read)) \
    __declspec(allocate(".CRT$XIU")) \
    static int(* CONCAT(fn,__2) )() = CONCAT(fn,__1); \
    static void fn(void)
#elif defined __TINYC__ // tcc...
#define AUTORUN_(fn) \
    __attribute((constructor)) \
    static void fn(void)
#else // gcc,clang,clang-cl...
#define AUTORUN_(fn) \
    __attribute__((constructor(__COUNTER__+101))) \
    static void fn(void)
#endif
// note: based on code by Joe Lowe (public domain).
// note: XIU for C initializers, XCU for C++ initializers, XTU for C deinitializers
#endif

#ifndef BUILD_VERSION
#define BUILD_VERSION              ( BUILD_YEAR * 10000 + BUILD_MONTH * 100 + BUILD_DAY ) // __DATE__ " " __TIME__
#define BUILD_YEAR ((((__DATE__ [7] - '0') * 10 + (__DATE__ [8] - '0')) * 10 + (__DATE__ [9] - '0')) * 10 + (__DATE__ [10] - '0'))
#define BUILD_DAY ((__DATE__ [4] == ' ' ? 0 : __DATE__ [4] - '0') * 10 + (__DATE__ [5] - '0'))
#define BUILD_MONTH (1+(__DATE__ [2] == 'n' ? 0 \
    : __DATE__ [2] == 'b' ? 1 \
    : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 2 : 3) \
    : __DATE__ [2] == 'y' ? 4 \
    : __DATE__ [2] == 'n' ? 5 \
    : __DATE__ [2] == 'l' ? 6 \
    : __DATE__ [2] == 'g' ? 7 \
    : __DATE__ [2] == 'p' ? 8 \
    : __DATE__ [2] == 't' ? 9 \
    : __DATE__ [2] == 'v' ? 10 : 11))
#endif

#ifndef CAST
#define CAST(type, ...)            ( ifdef(c,(type),type) { __VA_ARGS__ } )
#endif

#ifndef CONCAT
#define CONCAT(a,b)                CONC4T(a,b)
#define CONC4T(a,b)                a##b ///-
#endif

#ifndef COUNTOF
#define COUNTOF(x)                 (int)(sizeof (x) / sizeof 0[x])
#endif

#ifndef DEFER
#define DEFER(begin,end)           for(int MACRO(i) = ((begin), 0); !MACRO(i); MACRO(i) = ((end), 1))
#endif

#ifndef EXPAND
#define EXPAND(name, ...)          EXPAND_QUOTE(EXPAND_JOIN(name, EXPAND_COUNT_ARGS(__VA_ARGS__)), (__VA_ARGS__))
#define EXPAND_QUOTE(x, y)         x y ///-
#define EXPAND_JOIN(name, count)   EXPAND_J0IN(name, count) ///-
#define EXPAND_J0IN(name, count)   EXPAND_J01N(name, count) ///-
#define EXPAND_J01N(name, count)   name##count ///-
#define EXPAND_COUNT_ARGS(...)     EXPAND_ARGS((__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)) ///-
#define EXPAND_ARGS(args)          EXPAND_RETURN_COUNT args ///-
#define EXPAND_RETURN_COUNT(_1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, count, ...) count ///-
#endif

#ifndef EXTERN_C
#define EXTERN_C                   ifdef(c,,extern "C")
#endif

#ifndef FILELINE
#define FILELINE                   __FILE__ ":" STRINGIZE(__LINE__)
#endif

#ifndef FIXME
#define FIXME(str)                 ifdef(debug,WARNING("FIXME: " str " (" FILELINE ")"))
#endif

#ifndef FINITE
#define FINITE                     ifdef(cl, _finite, isfinite) // _finite if _MSC_VER <= 1700
#endif

#ifndef FORCE_INLINE
#define FORCE_INLINE               ifdef(cl, __forceinline, ifdef(gcc, __attribute__((always_inline)) inline, INLINE))
#endif

#ifndef INLINE
#define INLINE                     ifdef(cl, ifdef(c, __inline, inline), inline)
#endif

#ifndef MACRO
#define MACRO(name)                CONCAT(name, __LINE__)
#endif

#ifndef ONCE
#define ONCE                       static int MACRO(once) = 1; if( MACRO(once) && MACRO(once)-- ) // once = 0; for( ; !once && (once^=1) ; )
#endif

#ifndef SCOPE
#define SCOPE(end)                 DEFER((void)0, end)
#endif

#ifndef STATIC_ASSERT
#define STATIC_ASSERT(EXPR)        typedef struct { unsigned MACRO(static_assert_on_L) : !!(EXPR); } UNIQUE(static_assert_on_L)
#endif

#ifndef STRINGIZE
#define STRINGIZE(x)               STRING1ZE(x)
#define STRING1ZE(x)               #x ///-
#endif

#ifndef TODO
#define TODO(str)                  ifdef(debug,WARNING("TO DO: " str " (" FILELINE ")"))
#endif

#ifndef UNIQUE
#define UNIQUE(name)               CONCAT(CONCAT(CONCAT(name,CONCAT(_L,__LINE__)),_),__COUNTER__)
#endif

#ifndef VA_COUNT
#define VA_COUNT(...)              (int)(sizeof((int[]){0, ##__VA_ARGS__})/sizeof(int)-1)
#endif

#ifndef VA_FIRST
#define VA_FIRST(...)              VA_F1RST(__VA_ARGS__, throwaway)
#define VA_F1RST(first, ...)       first ///-
#endif

#ifndef VA_OTHER
#define VA_OTHER(...)              VA_0THER(VA_NUM(__VA_ARGS__), __VA_ARGS__)
#define VA_0THER(qty, ...)         VA_0TH3R(qty, __VA_ARGS__) ///-
#define VA_0TH3R(qty, ...)         VA_0TH3R_##qty(__VA_ARGS__) ///-
#define VA_0TH3R_ONE(first)        ///-
#define VA_0TH3R_MORETHAN1(first, ...) , __VA_ARGS__ ///-
#define VA_NUM(...)                VA_SELECT_10TH(__VA_ARGS__, MORETHAN1, MORETHAN1, MORETHAN1, MORETHAN1, MORETHAN1, MORETHAN1, MORETHAN1, MORETHAN1, ONE, throwaway) ///-
#define VA_SELECT_10TH(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, ...) A10 ///-
// VA_COUNT() counts number of va args
// VA_SPLIT() expands to A) 1 item OR B) 1 item + ',' + va_args[1..N]
// VA_FIRST() expands to the first argument
// VA_OTHER() if there's only one argument, expands to nothing.  if there is more
//            than one argument, expands to a comma followed by everything but
//            the first argument. supports up to 9 arguments, but can be expanded.
#endif

#ifndef VA_SPLIT
#define VA_SPLIT(...)              VA_FIRST(__VA_ARGS__) VA_OTHER(__VA_ARGS__)
#endif

#ifndef WARNING
#if is(gcc)
#define WARNING(msg)               WARN1NG( message( msg ) )
#define WARN1NG(msg)               _Pragma(#msg)
#elif is(cl)
#define WARNING(msg)               __pragma( message( msg ) )
#else
#define WARNING(msg)
#endif
#endif

#endif
