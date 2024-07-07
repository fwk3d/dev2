#if 0
#include <stdio.h>
#include <stdlib.h>

#undef REALLOC
#undef MALLOC
#undef FREE
#undef CALLOC
#undef STRDUP
#undef MSIZE

static __thread int xrealloc_cnt;
#define REALLOC xrealloc
#define MALLOC(sz) xrealloc(0,(sz))
#define CALLOC(c,sz) (xrealloc_cnt = (c)*(sz), memset(xrealloc(0,xrealloc_cnt),0,xrealloc_cnt))
#define FREE(p) xrealloc(0,(sz))
#define STRDUP(s) xrealloc(0,(sz))

#define ALLOCA(sz)

#define ALLOCSIZE(p)   xsize(p)
#define MALLOC(n)      REALLOC_(0,(n))
#define FREE(p)        REALLOC_((p), 0)
#define REALLOC(p,n)   REALLOC_((p),(n))
#define CALLOC(m,n)    CALLOC_((m),(n))
#define STRDUP(s)      STRDUP_(s)
#define ALLOCA(n)      ifdef(gcc, __builtin_alloca(n), ifdef(win32, _alloca(n), __builtin_alloca(n)))

#define STB_SPRINTF_IMPLEMENTATION
#include "3rd/3rd_stb_sprintf.h"

#define sprintf stbsp_sprintf
#define vsnprintf stbsp_vsnprintf
#endif
