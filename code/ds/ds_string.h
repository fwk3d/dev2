// string utilities,
// - rlyeh, public domain

#if !CODE

// string: temporary strings. DO NOT FREE()
API char* va(const char *fmt, ...);

// string: heap allocated strings. FREE() after use
API char*   strcatf(char **s, const char *buf);
#define     strcatf(s,fmt,...)  strcatf((s), va(fmt, __VA_ARGS__))
#define     stringf(fmt,...)    STRDUP(va(fmt, __VA_ARGS__)) // (strcatf)(0, va(fmt, __VA_ARGS__))

// search
API const char* strbeg(const char *a, const char *b); // returns initial match, if any. case insensitive
API const char* strend(const char *src, const char *sub); // returns final match, if any. case insensitive
API const char* strbegi(const char *a, const char *b); // returns initial match, if any. case insensitive
API const char* strendi(const char *src, const char *sub); // returns final match, if any. case insensitive
API const char* strstri(const char *a, const char *b);
#define         strcmpi  ifdef(cl, _stricmp, strcasecmp)

// search (wildcards)
API int   strmatch(const char *s, const char *wildcard);
API int   strmatchi(const char *s, const char *wildcard);

// conv
API char* strlower(const char *str);
API char* strupper(const char *str);

// trim
API char *strrepl(char **copy, const char *target, const char *replace); // replace any 'target' as 'repl' in 'copy'. 'copy' may change (heap). returns 'copy'
API char *strswap(char *copy, const char *target, const char *replace); // replaced only if new text is shorter than old one
API char *strcut(char *copy, const char *target);

// sort
API int qsort_strcmp(const void *a, const void *b);
API int qsort_strcmpi(const void *a, const void *b);

// string split/join ----------------------------------------------------------
/// split `string` after any of `delimiters` character is found.
/// returns temporary array of split strings. see: strjoin
/// > array(char*) tokens = strsplit("hello! world!", " !"); // [0]="hello",[1]="world",
API array(char*)    strsplit(const char *string, const char *delimiters);

/// concatenate all elements within `list`, with `separator` string in between.
/// returns: temporary joint string. see: strsplit
/// > array(char*) tokens = strsplit("hello! world!", " !"); // [0]="hello",[1]="world",
/// > char *joint = strjoin(tokens, "+"); // joint="hello+world"
API char*           strjoin(array(char*) list, const char *separator);

// iterator
#if 1
#define each_substring(str, delims, keyname) \
    ( char *str_ = (char*)(str); str_; str_ = 0 ) \
    for( int len_ = strlen(str_) + 1, heap_ = len_ < 1024; len_ > 1; len_ = 0 ) \
    for( char *ptr_ = (char*)(heap_ ? REALLOC(0, len_) : ALLOCA(len_)), *cpy_ = (snprintf(ptr_, len_, "%s", str_), ptr_); ptr_; (heap_ ? REALLOC(ptr_, 0) : 0), ptr_ = 0 ) \
    for( const char *next_token = 0, *keyname = strtok_r(cpy_, delims, (char**)&next_token); keyname; keyname = strtok_r(NULL, delims, (char**)&next_token) )
#else
#define each_substring(str, delims, keyname) \
    ( char** tokens_ = strsplit((str), (delims)), *keyname = 0; tokens_; tokens_ = 0) \
    for( int i_ = 0, end_ = array_count(tokens_); i_ < (keyname = tokens_[i_], end_); ++i_ )
#endif

// utils
#define   va(...) (((&printf) || printf(__VA_ARGS__), va(__VA_ARGS__)))  // vs2015 check trick

#if is(cl) || (is(tcc) && is(win32))
#define strtok_r strtok_s
#if!is(cl)
char* strtok_s(char* str,const char* delimiters,char** context); // tcc misses this in <string.h>
#endif
#endif

#else

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#if 0
char* (va)(const char *fmt, ...) {
    #ifndef VA_NUM_GENERATIONS
    #define VA_NUM_GENERATIONS 64 // increase this number if experiencing undefined behavior
    #endif
    static array(char*) buf[VA_NUM_GENERATIONS] = {0};
    static int l = 0; l = (l+1) % VA_NUM_GENERATIONS;
    va_list vl;
    va_start(vl,fmt);
        int rc = vsnprintf(NULL, 0, fmt, vl);
        ASSERT(rc >= 0);
        array_resize(buf[l], rc+1);
        rc = vsnprintf((char*)buf[l], rc+1, fmt, vl);
        ASSERT(rc);
    va_end(vl);
    return (char*)buf[l];
}
#else

#ifndef STACK_ALLOC_SIZE
#define STACK_ALLOC_SIZE (2048*1024)
#endif

#ifdef STB_SPRINTF_H_INCLUDE
#define my_vsnprintf stbsp_vsnprintf
#else
#define my_vsnprintf vsnprintf
#endif

char* tempvl(const char *fmt, va_list vl) {
    va_list copy;
    va_copy(copy, vl);
    int sz = my_vsnprintf( 0, 0, fmt, copy ) + 1;
    ASSERT(sz >= 1);
    va_end(copy);

    int reqlen = sz;
#if 0
    int heap = 0;
    enum { STACK_ALLOC = 16384 };
    static __thread char buf[STACK_ALLOC];
#else
    int heap = 1;
    static __thread int STACK_ALLOC = STACK_ALLOC_SIZE;
    static __thread char *buf = 0; if(!buf) buf = REALLOC(0, STACK_ALLOC); // @leak
#endif
    static __thread int cur = 0; //printf("string stack %d/%d\n", cur, STACK_ALLOC);

    if( reqlen >= STACK_ALLOC ) {
        printf(ANSI_RED("no stack enough, increase STACK_ALLOC variable above (reqlen:%d) (fmt: %s)\n"), reqlen, fmt);
        //ASSERT(reqlen < STACK_ALLOC);
        STACK_ALLOC = reqlen * 2;
        buf = REALLOC(0, STACK_ALLOC); // @leak
        ASSERT(buf);
    }

    char* ptr = buf + (cur *= (cur+reqlen) < (STACK_ALLOC - 1), (cur += reqlen) - reqlen);

    int rc = my_vsnprintf( ptr, sz, fmt, vl );
        ASSERT(ptr);
        ASSERT(sz);
        ASSERT(rc >= 0);
    return (char *)ptr;
}
char* /*tempva*/(va)(const char *fmt, ...) {
    va_list vl;
    va_start(vl, fmt);
    char *s = tempvl(fmt, vl);
    va_end(vl);
    return s;
}

#endif

char* (strcatf)(char **src_, const char *buf) {
    char *src = *src_;
        if(!buf) return src;
        // if(!buf[0]) return src;
        int srclen = (src ? strlen(src) : 0), buflen = strlen(buf);
        src = (char*)REALLOC(src, srclen + buflen + 1 );
        memcpy(src + srclen, buf, buflen + 1 );
    *src_ = src;
    return src;
}

int strmatch(const char *s, const char *wildcard) {
    // returns true if wildcard matches
    if( *wildcard == '\0' ) return !*s;
    if( *wildcard ==  '*' ) return strmatch(s, wildcard+1) || (*s && strmatch(s+1, wildcard));
    if( *wildcard ==  '?' ) return *s && (*s != '.') && strmatch(s+1, wildcard+1);
    return (*s == *wildcard) && strmatch(s+1, wildcard+1);
}
int strmatchi(const char *s, const char *wildcard) {
    // returns true if wildcard matches (case insensitive)
    if( *wildcard == '\0' ) return !*s;
    if( *wildcard ==  '*' ) return strmatchi(s, wildcard+1) || (*s && strmatchi(s+1, wildcard));
    if( *wildcard ==  '?' ) return *s && (*s != '.') && strmatchi(s+1, wildcard+1);
    return (tolower(*s) == tolower(*wildcard)) && strmatchi(s+1, wildcard+1);
}


const char *strstri( const char *src, const char *sub ){
    while( *src++ ) {
        for( const char *s = src-1, *f = sub, *c = s; ; ++f, ++c) {
            if(!*f) return s;
            if(!*c) return NULL;
            if(tolower(*c) != tolower(*f)) break;
        }
    }
    return NULL;
}
const char *strendi(const char *src, const char *sub) { // returns final match, if any. case insensitive
    int srclen = strlen(src);
    int sublen = strlen(sub);
    if( sublen > srclen ) return 0;
    return !strcmpi(src + srclen - sublen, sub) ? src + srclen - sublen : NULL;
}
const char *strend(const char *src, const char *sub) { // returns final match, if any. case insensitive
    int srclen = strlen(src);
    int sublen = strlen(sub);
    if( sublen > srclen ) return 0;
    return !strcmp(src + srclen - sublen, sub) ? src + srclen - sublen : NULL;
}
const char *strbegi(const char *a, const char *b) { // returns initial match, if any. case insensitive
    int la = strlen(a), lb = strlen(b);
    if( la < lb ) return NULL;
    if( lb == 0 ) return a;
    int len = la < lb ? la : lb;
    for( int i = 0; i < len; ++i ) {
        if( tolower(a[i]) != tolower(b[i]) ) {
            return NULL;
        }
    }
    return a;
}
const char *strbeg(const char *a, const char *b) { // returns initial match, if any. case insensitive
    int la = strlen(a), lb = strlen(b);
    if( la < lb ) return NULL;
    if( lb == 0 ) return a;
    int len = la < lb ? la : lb;
    for( int i = 0; i < len; ++i ) {
        if( a[i] != b[i] ) {
            return NULL;
        }
    }
    return a;
}

char *strupper(const char *str) {
    char *s = va("%s", str), *bak = s;
    while(*s++) s[-1] = toupper(s[-1]);
    return bak;
}
char *strlower(const char *str) {
    char *s = va("%s", str), *bak = s;
    while(*s++) s[-1] = tolower(s[-1]);
    return bak;
}

char *strrepl(char **string, const char *target, const char *replace) { // may reallocate input string if needed
    //if new text is shorter than old one,then no need to heap, replace inline
    int rlen = strlen(replace), tlen = strlen(target), diff = tlen - rlen;
    if( diff >= 0 ) return strswap(*string, target, replace);

    char *buf = 0, *aux = *string;
    for( int tgtlen = tlen; tgtlen && aux[0]; ) {
        char *found = strstr(aux, target);
        if( found ) {
            strcatf(&buf, "%.*s%s", (int)(found - aux), aux, replace);
            aux += (found - aux) + tgtlen;
        } else {
            strcatf(&buf, "%s", aux);
            break;
        }
    }
    if( buf ) {
        // strcpy(*string, buf);
        char *s = *string;
        if(s) s[0] = 0;
        strcatf(&s, "%s", buf);
        *string = s;

        FREE( buf );
    }
    return *string;
}
char *strswap(char *copy, const char *target, const char *replacement) {
    // replaced only if new text is shorter than old one
    int rlen = strlen(replacement), diff = strlen(target) - rlen;
    if( diff >= 0 ) {
        for( char *s = copy, *e = s + strlen(copy); /*s < e &&*/ 0 != (s = strstr(s, target)); ) {
            if( rlen ) s = (char*)memcpy( s, replacement, rlen ) + rlen;
            if( diff ) memmove( s, s + diff, (e - (s + diff)) + 1 );
        }
    }
    return copy;
}
char *strcut(char *copy, const char *target) {
    return strswap(copy, target, "");
}

// -----------------------------------------------------------------------------
// string utils

int qsort_strcmp(const void *a, const void *b) {
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcmp(*ia, *ib);
}
int qsort_strcmpi(const void *a, const void *b) {
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcmpi(*ia, *ib);
}

// -----------------------------------------------------------------------------
// string split/join

array(char*) strsplit(const char *str, const char *separators) {
    enum { SLOTS = 32 };
    static __thread int slot = 0;
    static __thread char *buf[SLOTS] = {0};
    static __thread array(char*) list[SLOTS] = {0};

    slot = (slot+1) % SLOTS;
    array_resize(list[slot], 0);
    *(buf[slot] = REALLOC(buf[slot], strlen(str)*2+1)) = '\0'; // *2 to backup pathological case where input str is only separators && include == 1

    for(char *dst = buf[slot]; str && *str; ) {
        // count literal run && terminators
        int run = strcspn(str, separators);
        int end = strspn(str + run, separators);

        // append literal run
        if( run ) {
            array_push(list[slot], dst);
            memmove(dst,str,run); dst[run] = '\0'; //strncpy(dst, str, run)
            dst += run + 1;
        }
#if defined SPLIT_INCLUDE_SEPARATORS && SPLIT_INCLUDE_SEPARATORS
        // mode: append all separators: "1++2" -> "1" "+" "+" "2"
        for( int i = 0; i < end; ++i ) {
            array_push(list[slot], dst);
            dst[0] = str[ run + i ];
            dst[1] = '\0';
            dst += 2;
        }
#endif

        // skip both
        str += run + end;
    }

    return list[slot];
}
char* strjoin(array(char*) list, const char *separator) {
    enum { SLOTS = 16 };
    static __thread int slot = 0;
    static __thread char* mems[SLOTS] = {0};

    slot = (slot+1) % SLOTS;

    int num_list = array_count(list);
    int len = 0, inc = 0, seplen = strlen(separator);
    for( int i = 0; (num_list > 0 ? i < num_list : !!list[i]); ++i ) {
        len += strlen(list[i]) + inc;
        inc = seplen;
    }

    mems[slot] = REALLOC(mems[slot], len+1);
    char *p = mems[slot]; *p = 0;
    const char *sep = "";
    for( int i = 0; (num_list > 0 ? i < num_list : !!list[i]); ++i ) {
        p += sprintf(p, "%s%s", sep, list[i]);
        sep = separator;
    }

    return mems[slot];
}

#endif
