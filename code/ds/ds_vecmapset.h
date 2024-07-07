// data structures and utils: array/vector, set, map.
// - rlyeh, public domain

#if !CODE

// -----------------------------------------------------------------------------
// less

API int less_64(uint64_t a, uint64_t b);
API int less_int(int a, int b);
API int less_ptr(void *a, void *b);
API int less_str(char *a, char *b);

// -----------------------------------------------------------------------------
// qsort

API int less_64_ptr(const void *a, const void *b);
API int less_int_ptr(const void *a, const void *b);

// -----------------------------------------------------------------------------
// vector based allocator (x1.75 enlarge factor)

API void* vrealloc( void* p, size_t sz );
API size_t vlen( void* p );

// -----------------------------------------------------------------------------
// arrays

#if is(cpp)
#define array_cast(x) (decltype x)
#else
#define array_cast(x) (void *)
#endif

#define array(t) t*
#define array_init(t) ( (t) = 0 )
#define array_resize(t, n) ( array_c_ = array_count(t), array_n_ = (n), array_realloc_((t),array_n_), (array_n_>array_c_? memset(array_c_+(t),0,(array_n_-array_c_)*sizeof(0[t])) : (void*)0), (t) )
#define array_push(t, ...) ( array_realloc_((t),array_count(t)+1), (t)[ array_count(t) - 1 ] = (__VA_ARGS__) )
#define array_pop(t) ( array_realloc_((t), array_count(t)-1) )
#define array_back(t) ( &(t)[ array_count(t)-1 ] ) // ( (t) ? &(t)[ array_count(t)-1 ] : NULL )
#define array_data(t) (t)
#define array_at(t,i) (t[i])
#define array_count(t) (int)( (t) ? array_vlen_(t) / sizeof(0[t]) : 0u )
#define array_bytes(t) (int)( (t) ? array_vlen_(t) : 0u )
#define array_sort(t, cmpfunc) qsort( t, array_count(t), sizeof(0[t]), (uintptr_t)cmpfunc == (uintptr_t)strcmp ? (void*)qsort_strcmp : (void*)cmpfunc )
#define array_empty(t) ( !array_count(t) )

#define array_push_front(arr,x) \
    (array_resize((arr), array_count(arr)+1), memmove((arr)+1, (arr), sizeof(0[arr])*array_count(arr)), 0[arr] = (x))
#define array_pop_front(arr) ( \
    (array_count(arr) > 1 ? memmove((arr), (arr)+1, sizeof(0[arr])*(array_count(arr)-1)) : (void*)0), \
    (array_count(arr) > 0 ? array_resize(arr, array_count(arr) - 1 ) : array_resize( arr, 0 ) ) )

static __thread unsigned array_c_;
static __thread unsigned array_n_;

#if 0 // original: no reserve support
#define array_reserve(t, n) ((void)0) // not implemented
#define array_clear(t) ( array_realloc_((t), 0), (t) = 0 )
#define array_vlen_(t)  ( vlen(t) - 0 )
#define array_realloc_(t,n)  ( (t) = array_cast(t) vrealloc((t), ((n)+0) * sizeof(0[t])) )
#define array_free(t) array_clear(t)
#else // new: with reserve support (@todo: check for bugs?)
#define array_reserve(t, n) ( array_realloc_((t),(n)), array_clear(t) )
#define array_clear(t) ( array_realloc_((t),0) ) // -1
#define array_vlen_(t)  ( vlen(t) - sizeof(0[t]) ) // -1
#define array_realloc_(t,n)  ( (t) = array_cast(t) vrealloc((t), ((n)+1) * sizeof(0[t])) ) // +1
#define array_free(t) ( array_realloc_((t), -1), (t) = 0 ) // -1
#endif

#define array_reverse(t) \
    do if( array_count(t) ) { \
        for(int l = array_count(t), e = l-1, i = (array_push(t, 0[t]), 0); i <= e/2; ++i ) \
            { l[t] = i[t]; i[t] = (e-i)[t]; (e-i)[t] = l[t]; } \
        array_pop(t); \
    } while(0)

#define array_foreach(t,val_t,v) for each_array(t,val_t,v)
#define each_array(a,val_t,v) \
    ( array(val_t) a_ = (a); a_; a_ = 0 ) \
    for( int i_ = 0, e_ = array_count(a_); i_ < e_; ++i_ ) \
        for( val_t v = i_[a_], *v_ = (void*)(uintptr_t)&v; v_; v_ = 0 )

#define array_foreach_ptr(t,val_t,v) for each_array_ptr(t,val_t,v)
#define each_array_ptr(a,val_t,v) \
    ( array(val_t) a_ = (a); a_; a_ = 0 ) \
    for( int i_ = 0, e_ = array_count(a_); i_ < e_; ++i_ ) \
        for( val_t *v = (val_t*)&i_[a_]; v; v = 0 )

#define array_search(t, key, cmpfn) /* requires sorted array beforehand */ \
    bsearch(&key, t, array_count(t), sizeof(t[0]), cmpfn )

#define array_insert(t, i, n) do { \
    int ac = array_count(t); \
    if( i >= ac ) { \
        array_push(t, n); \
    } else { \
        array_push(t, array_back(t)); \
        memmove( &(t)[(i)+1], &(t)[i], (ac - (i)) * sizeof(t[0]) ); \
        (t)[ i ] = (n); \
    } \
} while(0)

#define array_copy(t, src) do { \
    array_free(t); \
    (t) = array_realloc_( (t), array_count(src)); \
    memcpy( (t), src, array_count(src) * sizeof(0[t])); \
} while(0)

#define array_erase_fast(t, i) do { /*alters ordering*/ \
    memcpy( &(t)[i], &(t)[array_count(t) - 1], sizeof(0[t])); \
    array_pop(t); \
} while(0)

#define array_erase_slow(t, i) do { /*preserves ordering*/ \
    memmove( &(t)[i], &(t)[i + 1], sizeof(0[t])*(array_count(t) - i - 1)); \
    array_pop(t); \
} while(0)

#define array_unique(t, cmpfunc) do { /*@todo: optimize me. requires array_sort() beforehand*/ \
    int cnt = array_count(t), cnt_bak = cnt; \
     if( cnt > 1 ) { \
        for( int i = 1; i < cnt; ++i ) { \
            while( i < cnt && !cmpfunc(&(t)[i-1], &(t)[i]) ) { \
                memmove(&(t)[i-1], &(t)[i], (cnt - 1 - i) * sizeof((t)[0]) ) ; \
                --cnt; \
            } \
         } \
        if( cnt_bak != cnt ) array_resize((t), cnt); \
     } \
 } while(0)

#if 0 // snippet below does work
#define array_unique(t, cmpfunc) \
    array_sort(t, cmpfunc); \
    for( int i = 0, end = array_count(t) - 1; i < end; ) { \
        if( !strcmp(t[i], t[i+1]) ) { \
            /* array_erase(t, i+1); */ \
            memmove(&(t)[i+1], &(t)[i+2], (end - 1 - i) * sizeof((t)[0]) ); \
            array_pop(t); \
            --end; \
        } else { \
            ++i; \
        } \
    }
#endif

#define array_shuffle(t) do { /* https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle */ \
    void* tmp = stack(sizeof(0[t])); \
    for( int i = 0, n = array_count(t); i < n; ++i ) { \
        int j = randi(i, n); /* j random integer such that [i,n) i<=j<n */ \
        memcpy(tmp, &j[t], sizeof(0[t])); \
        memcpy(&j[t], &i[t], sizeof(0[t])); \
        memcpy(&i[t], tmp, sizeof(0[t])); \
    } \
} while(0)

// -----------------------------------------------------------------------------
// set<K>
// ideas from: https://en.wikipedia.org/wiki/Hash_table
// ideas from: https://probablydance.com/2017/02/26/i-wrote-the-fastest-hashtable/
// ideas from: http://www.idryman.org/blog/2017/05/03/writing-a-damn-fast-hash-table-with-tiny-memory-footprints/

// config
#ifndef SET_HASHSIZE
#define SET_HASHSIZE (4096 << 4)
#endif
#ifndef SET_DONT_ERASE
#define SET_DONT_ERASE 1
#endif

// public api
#define set(K) \
    struct { set base; struct { set_item p; K key; } tmp, *ptr; K *tmpval; \
        int (*typed_cmp)(K, K); uint64_t (*typed_hash)(K); } *

#define set_init(m, cmpfn, hashfn) ( \
    (m) = set_cast(m) REALLOC(0, sizeof(*m)), \
    set_init(&(m)->base), \
    (m)->base.cmp = (int(*)(void*,void*))( (m)->typed_cmp = set_cast(cmpfn) cmpfn ), \
    (m)->base.hash = (uint64_t(*)(void*))( (m)->typed_hash = set_cast(hashfn) hashfn ) \
    )

#define set_free(m) ( \
    set_clear(m), \
    set_free(&(m)->base), \
    (m) = set_cast(m) REALLOC((m), 0), \
    (m) = 0 \
    )

#define set_insert(m, k) ( \
    (m)->ptr = set_cast((m)->ptr) REALLOC(0, sizeof((m)->tmp)), \
    (m)->ptr->p.keyhash = (m)->typed_hash((m)->ptr->key = (k)), \
    set_insert(&(m)->base, &(m)->ptr->p, &(m)->ptr->key, (m)->ptr->p.keyhash, (m)->ptr), \
    &(m)->ptr->key \
    )

#define set_find(m, k) ( \
    (m)->ptr = &(m)->tmp, \
    (m)->ptr->p.keyhash = (m)->typed_hash((m)->ptr->key = (k)), \
    (m)->ptr = set_cast((m)->ptr) set_find(&(m)->base, &(m)->ptr->key, (m)->ptr->p.keyhash), \
    (m)->ptr ? &(m)->ptr->key : 0 \
    )

#define set_find_or_add(m, k) ( \
    (m)->tmp.key = (k), \
    (m)->tmpval = set_find((m), ((m)->tmp.key)), \
    (m)->tmpval = (m)->tmpval ? (m)->tmpval : set_insert((m), ((m)->tmp.key)) \
    )

#define set_find_or_add_allocated_key(m, k) ( \
    (m)->tmp.key = (k), \
    (m)->tmpval = set_find((m), ((m)->tmp.key)), \
    (m)->tmpval = (m)->tmpval ? FREE((m)->tmp.key), (m)->tmpval : set_insert((m), ((m)->tmp.key)) \
    )

#define set_erase(m, k) ( \
    (m)->ptr = &(m)->tmp, \
    (m)->ptr->p.keyhash = (m)->typed_hash((m)->ptr->key = (k)), \
    set_erase(&(m)->base, &(m)->ptr->key, (m)->ptr->p.keyhash) \
    )

#define set_foreach for each_set
#define each_set(m,key_t,k) \
    ( int i_ = (m)->base.count ? 0 : SET_HASHSIZE; i_ < SET_HASHSIZE; ++i_) \
        for( set_item *cur_ = (m)->base.array[i_], *on_ = cur_; cur_; on_ = cur_ = cur_->next ) \
            for( key_t k = *(key_t *)cur_->key; on_; on_ = 0 )

#define set_foreach_ptr for each_set_ptr
#define each_set_ptr(m,key_t,k) \
    ( int i_ = (m)->base.count ? 0 : SET_HASHSIZE; i_ < SET_HASHSIZE; ++i_) \
        for( set_item *cur_ = (m)->base.array[i_], *on_ = cur_; cur_; on_ = cur_ = cur_->next ) \
            for( key_t *k = (key_t *)cur_->key; on_; on_ = 0 )

#define set_clear(m) ( \
    set_clear(&(m)->base) \
    )

#define set_isempty(m)      set_isempty(&(m)->base)
#define set_count(m)        set_count(&(m)->base)
#define set_gc(m)           set_gc(&(m)->base)

#ifndef set_init_int
#define set_init_int(m) set_init((m), less_int, hash_64) // hash_int)
#define set_init_str(m) set_init((m), less_str, hash_str)
#define set_init_ptr(m) set_init((m), less_ptr, hash_ptr)
#endif

// private:

#if is(cpp)
#define set_cast(t) (decltype(t))
#else
#define set_cast(t) (void *)
#endif

typedef struct set_item {
    struct set_item *next;

    uint64_t keyhash;
    void *key;
    void *super;
} set_item;

typedef struct set {
    array(set_item*) array;
    int (*cmp)(void *, void *);
    uint64_t (*hash)(void *);
    int count;
} set;

API void  (set_init)(set *m);
API void  (set_free)(set *m);

API void  (set_insert)(set *m, set_item *p, void *key, uint64_t keyhash, void *super);
API void  (set_erase)(set *m, void *key, uint64_t keyhash);
API void* (set_find)(const set *m, void *key, uint64_t keyhash);
API int   (set_isempty)(const set *m);
API int   (set_count)(const set *m);
API void  (set_gc)(set *m); // only if using SET_DONT_ERASE
API void  (set_clear)(set* m);

// -----------------------------------------------------------------------------
// map<K,V>
// ideas from: https://en.wikipedia.org/wiki/Hash_table
// ideas from: https://probablydance.com/2017/02/26/i-wrote-the-fastest-hashtable/
// ideas from: http://www.idryman.org/blog/2017/05/03/writing-a-damn-fast-hash-table-with-tiny-memory-footprints/

// config
#ifndef MAP_HASHSIZE
#define MAP_HASHSIZE (4096 << 4)
#endif
#ifndef MAP_DONT_ERASE
#define MAP_DONT_ERASE 1
#endif

// public api
#define map(K,V) \
    struct { map base; struct { pair p; K key; V val; } tmp, *ptr; V* tmpval; \
        int (*typed_cmp)(K, K); uint64_t (*typed_hash)(K); } *

#define map_init(m, cmpfn, hashfn) ( \
    (m) = map_cast(m) REALLOC(0, sizeof(*(m))), \
    map_init(&(m)->base), \
    (m)->base.cmp = (int(*)(void*,void*))( (m)->typed_cmp = map_cast((m)->typed_cmp) cmpfn), \
    (m)->base.hash = (uint64_t(*)(void*))( (m)->typed_hash = map_cast((m)->typed_hash) hashfn ) \
    )

#define map_free(m) ( \
    map_free(&(m)->base), \
    map_cast(m) REALLOC((m), sizeof(*(m))), (m) = 0 \
    )

#define map_insert(m, k, v) ( \
    (m)->ptr = map_cast((m)->ptr) REALLOC(0, sizeof((m)->tmp)), \
    (m)->ptr->val = (v), \
    (m)->ptr->p.keyhash = (m)->typed_hash((m)->ptr->key = (k)), \
    map_insert(&(m)->base, &(m)->ptr->p, &(m)->ptr->key, &(m)->ptr->val, (m)->ptr->p.keyhash, (m)->ptr), \
    &(m)->ptr->val \
    )

#define map_find(m, k) ( \
    (m)->ptr = &(m)->tmp, \
    (m)->ptr->p.keyhash = (m)->typed_hash((m)->ptr->key = (k)), \
    (m)->ptr = map_cast((m)->ptr) map_find(&(m)->base, &(m)->ptr->key, (m)->ptr->p.keyhash), \
    (m)->ptr ? &(m)->ptr->val : 0 \
    )

#define map_find_or_add(m, k, v) ( \
    (m)->tmp.key = (k), (m)->tmp.val = (v), \
    (m)->tmpval = map_find((m), ((m)->tmp.key)), \
    (m)->tmpval = (m)->tmpval ? (m)->tmpval : map_insert((m), ((m)->tmp.key), ((m)->tmp.val)) \
    )

#define map_find_or_add_allocated_key(m, k, v) ( \
    (m)->tmp.key = (k), (m)->tmp.val = (v), \
    (m)->tmpval = map_find((m), ((m)->tmp.key)), \
    (m)->tmpval = (m)->tmpval ? FREE((m)->tmp.key), (m)->tmpval : map_insert((m), ((m)->tmp.key), ((m)->tmp.val)) \
    )

#define map_erase(m, k) ( \
    (m)->ptr = &(m)->tmp, \
    (m)->ptr->p.keyhash = (m)->typed_hash((m)->ptr->key = (k)), \
    map_erase(&(m)->base, &(m)->ptr->key, (m)->ptr->p.keyhash) \
    )

#define map_foreach for each_map
#define each_map(m,key_t,k,val_t,v) \
    ( int i_ = (m)->base.count ? 0 : MAP_HASHSIZE; i_ < MAP_HASHSIZE; ++i_) \
        for( pair *cur_ = (m)->base.array[i_], *on_ = cur_; cur_; on_ = cur_ = cur_->next ) \
            for( key_t k = *(key_t *)cur_->key; on_; ) \
                for( val_t v = *(val_t *)cur_->value; on_; on_ = 0 )

#define map_foreach_ptr for each_map_ptr
#define each_map_ptr(m,key_t,k,val_t,v) \
    ( int i_ = (m)->base.count ? 0 : MAP_HASHSIZE; i_ < MAP_HASHSIZE; ++i_) \
        for( pair *cur_ = (m)->base.array[i_], *on_ = cur_; cur_; on_ = cur_ = cur_->next ) \
            for( key_t *k = (key_t *)cur_->key; on_; ) \
                for( val_t *v = (val_t *)cur_->value; on_; on_ = 0 )

#define map_foreach_ptr_sorted for each_map_ptr_sorted
#define each_map_ptr_sorted(m,key_t,k,val_t,v) \
    ( int i_ = (map_sort(&(m)->base), 0); i_ < array_count((m)->base.sorted); ++i_) \
        for( pair *cur_ = (m)->base.sorted[i_]; cur_; ) \
            for( key_t *k = (key_t *)cur_->key; cur_; ) \
                for( val_t *v = (val_t *)cur_->value; cur_; cur_ = 0 )

#define map_clear(m) ( \
    map_clear(&(m)->base) \
    )

#define map_isempty(m)      map_isempty(&(m)->base)
#define map_count(m)        map_count(&(m)->base)
#define map_gc(m)           map_gc(&(m)->base)

// aliases:

#ifndef map_init_int
#define map_init_int(m)     map_init((m), less_int, hash_int) // hash_64
#define map_init_str(m)     map_init((m), less_str, hash_str)
#define map_init_ptr(m)     map_init((m), less_ptr, hash_ptr)
#endif

// private:

#if is(cpp)
#define map_cast(t) (decltype(t))
#else
#define map_cast(t) (void *)
#endif

typedef struct pair {
    struct pair *next;

    uint64_t keyhash;
    void *key;
    void *value;
    void *super;
} pair;

typedef struct map {
    array(pair*) array;
    int (*cmp)(void *, void *);
    uint64_t (*hash)(void *);
    int count:31;

    int is_sorted:1;
    array(pair*) sorted;
} map;

API void  (map_init)(map *m);
API void  (map_free)(map *m);

API void  (map_insert)(map *m, pair *p, void *key, void *value, uint64_t keyhash, void *super);
API void  (map_erase)(map *m, void *key, uint64_t keyhash);
API void* (map_find)(map *m, void *key, uint64_t keyhash);
API int   (map_isempty)(map *m);
API int   (map_count)(map *m);
API void  (map_gc)(map *m); // only if using MAP_DONT_ERASE
API bool  (map_sort)(map* m);
API void  (map_clear)(map* m);

#else

// -----------------------------------------------------------------------------
// less/sort

int less_64_ptr(const void *a, const void *b) {
    return 0[(uint64_t*)a] - 0[(uint64_t*)b];
}
int less_int_ptr(const void *a, const void *b) {
    return 0[(int*)a] - 0[(int*)b];
}

int less_int(int a, int b) {
    return a - b;
}
int less_64(uint64_t a, uint64_t b) {
    return a > b ? +1 : -!!(a - b);
}
int less_ptr(void *a, void *b) {
    return (uintptr_t)a > (uintptr_t)b ? +1 : -!!((uintptr_t)a - (uintptr_t)b);
}
int less_str(char *a, char *b) {
    return strcmp((const char *)a, (const char *)b);
}

// -----------------------------------------------------------------------------
// vector based allocator (x1.75 enlarge factor)

void* vrealloc( void* p, size_t sz ) {
    if( !sz ) {
        if( p ) {
            size_t *ret = (size_t*)p - 2;
            ret[0] = 0;
            ret[1] = 0;
            REALLOC( ret, 0 );
        }
        return 0;
    } else {
        size_t *ret;
        if( !p ) {
            ret = (size_t*)REALLOC( 0, sizeof(size_t) * 2 + sz );
            ret[0] = sz;
            ret[1] = 0;
        } else {
            ret = (size_t*)p - 2;
            size_t osz = ret[0];
            size_t ocp = ret[1];
            if( sz <= (osz + ocp) ) {
                ret[0] = sz;
                ret[1] = ocp - (sz - osz);
            } else {
                ret = (size_t*)REALLOC( ret, sizeof(size_t) * 2 + sz * 1.75 );
                ret[0] = sz;
                ret[1] = (size_t)(sz * 1.75) - sz;
            }
        }
        return &ret[2];
    }
}
size_t vlen( void* p ) {
    return p ? 0[ (size_t*)p - 2 ] : 0;
}

// -----------------------------------------------------------------------------

enum { MAP_GC_SLOT = MAP_HASHSIZE };
typedef int map_is_pow2_assert[ !(MAP_HASHSIZE & (MAP_HASHSIZE - 1)) ? 1:-1];

static int map_get_index(uint64_t hkey1) {
    return hkey1 & (MAP_HASHSIZE-1);
}

void (map_init)(map* m) {
    map c = {0};
    *m = c;

    array_resize(m->array, (MAP_HASHSIZE+1));
    memset(m->array, 0, (MAP_HASHSIZE+1) * sizeof(m->array[0]) ); // array_resize() just did memset()
}

void (map_insert)(map* m, pair *p, void *key, void *value, uint64_t keyhash, void *super) {
    p->keyhash = keyhash;
    p->key = key;
    p->value = value;
    p->super = super;

    /* Insert onto the beginning of the list */
    int index = map_get_index(p->keyhash);
    p->next = m->array[index];
    m->array[index] = p;
    m->is_sorted = 0;

    ++m->count;
}

void* (map_find)(map* m, void *key, uint64_t keyhash) {
    int index = map_get_index(keyhash);
    for( pair *cur = m->array[index]; cur; cur = cur->next ) {
        if( cur->keyhash == keyhash ) {
            char **c = (char **)cur->key;
            char **k = (char **)key;
            if( !m->cmp(c[0], k[0]) ) {
                return cur->super;
            }
        }
    }
    return 0;
}

void (map_erase)(map* m, void *key, uint64_t keyhash) {
    int index = map_get_index(keyhash);
    for( pair *prev = 0, *cur = m->array[index]; cur; (prev = cur), (cur = cur->next) ) {
        if( cur->keyhash == keyhash ) {
            char **c = (char **)cur->key;
            char **k = (char **)key;
            if( !m->cmp(c[0], k[0]) ) {
                if( prev ) prev->next = cur->next; else m->array[index] = cur->next ? cur->next : 0;
#if MAP_DONT_ERASE
                /* Insert onto the beginning of the GC list */
                cur->next = m->array[MAP_GC_SLOT];
                m->array[MAP_GC_SLOT] = cur;
#else
                REALLOC(cur,0);
#endif
                --m->count;
                m->is_sorted = 0;
                return;
            }
        }
    }
}

int (map_count)(map* m) { // clean deferred GC_SLOT only
    return m->count;

    int counter = 0;
    for( int i = 0; i < MAP_HASHSIZE; ++i) {
        for( pair *cur = m->array[i]; cur; cur = cur->next ) {
            ++counter;
        }
    }
    return counter;
}

int (map_isempty)(map* m) { // clean deferred GC_SLOT only
    return !m->count;
}

void (map_gc)(map* m) { // clean deferred GC_SLOT only
#if MAP_DONT_ERASE
    for( pair *next, *cur = m->array[MAP_GC_SLOT]; cur; cur = next ) {
        next = cur->next;
        REALLOC(cur,0);
    }
    m->array[MAP_GC_SLOT] = 0;
#endif
}

void (map_clear)(map* m) {
    for( int i = 0; i <= MAP_HASHSIZE; ++i) {
        for( pair *next, *cur = m->array[i]; cur; cur = next ) {
            next = cur->next;
            REALLOC(cur,0);
        }
        m->array[i] = 0;
    }
    m->count = 0;
    m->is_sorted = 0;
}

bool (map_sort)(map* m) {
    if( m->is_sorted ) return false;

    array_clear(m->sorted);
//    array_reserve(m->sorted, m->count);

    for( int i = 0; i < array_count(m->array); ++i) {
        for( pair *cur = m->array[i]; cur; cur = cur->next ) {
            array_push(m->sorted, cur);
        }
    }

#if 0
    array_sort(m->sorted, m->cmp);
#else
    // @fixme: do better than bubble sort below
    for( int i = 0; i < array_count(m->sorted) - 1; ++i)
    for( int j = i+1; j < array_count(m->sorted); ++j) {
        pair *curi = m->sorted[i];
        pair *curj = m->sorted[j];
        char **c = (char **)curi->key;
        char **k = (char **)curj->key;
        if( m->cmp(c[0], k[0]) > 0 ) {
            pair *swap = m->sorted[i];
            m->sorted[i] = m->sorted[j];
            m->sorted[j] = swap;
        }
    }
#endif

    return m->is_sorted = true;
}

void (map_free)(map* m) {
    (map_clear)(m);

    array_free(m->array);
    m->array = 0;

    map c = {0};
    *m = c;
}

// -----------------------------------------------------------------------------

enum { set_GC_SLOT = SET_HASHSIZE };
typedef int set_is_pow2_assert[ !(SET_HASHSIZE & (SET_HASHSIZE - 1)) ? 1:-1];

static int set_get_index(uint64_t hkey1) {
    return hkey1 & (SET_HASHSIZE-1);
}

void (set_init)(set* m) {
    set zero = {0};
    *m = zero;

    array_resize(m->array, (SET_HASHSIZE+1));
    memset(m->array, 0, (SET_HASHSIZE+1) * sizeof(m->array[0]) ); // array_resize() just did memset()
}

void (set_insert)(set* m, set_item *p, void *key, uint64_t keyhash, void *super) {
    p->keyhash = keyhash;
    p->key = key;
    p->super = super;

    /* Insert onto the beginning of the list */
    int index = set_get_index(p->keyhash);
    p->next = m->array[index];
    m->array[index] = p;

    ++m->count;
}

void* (set_find)(const set* m, void *key, uint64_t keyhash) {
    int index = set_get_index(keyhash);
    for( const set_item *cur = m->array[index]; cur; cur = cur->next ) {
        if( cur->keyhash == keyhash ) {
            char **c = (char **)cur->key;
            char **k = (char **)key;
            if( !m->cmp(c[0], k[0]) ) {
                return cur->super;
            }
        }
    }
    return 0;
}

void (set_erase)(set* m, void *key, uint64_t keyhash) {
    int index = set_get_index(keyhash);
    for( set_item *prev = 0, *cur = m->array[index]; cur; (prev = cur), (cur = cur->next) ) {
        if( cur->keyhash == keyhash ) {
            char **c = (char **)cur->key;
            char **k = (char **)key;
            if( !m->cmp(c[0], k[0]) ) {
                if (prev) prev->next = cur->next; else m->array[index] = cur->next ? cur->next : 0;
#if SET_DONT_ERASE
                /* Insert onto the beginning of the GC list */
                cur->next = m->array[set_GC_SLOT];
                m->array[set_GC_SLOT] = cur;
#else
                REALLOC(cur,0);
#endif
                --m->count;
                return;
            }
        }
    }
}

int (set_count)(const set* m) { // does not include GC_SLOT
    return m->count;

    int counter = 0;
    for( int i = 0; i < SET_HASHSIZE; ++i) {
        for( const set_item *cur = m->array[i]; cur; cur = cur->next ) {
            ++counter;
        }
    }
    return counter;
}

int (set_isempty)(const set *m) { // clean deferred GC_SLOT only
    return !m->count;
}

void (set_gc)(set* m) { // clean deferred GC_SLOT only
#if SET_DONT_ERASE
    for( set_item *next, *cur = m->array[set_GC_SLOT]; cur; cur = next ) {
        next = cur->next;
        REALLOC(cur,0);
    }
    m->array[set_GC_SLOT] = 0;
#endif
}

void (set_clear)(set* m) { // include GC_SLOT
    for( int i = 0; i <= SET_HASHSIZE; ++i) {
        for( set_item *next, *cur = m->array[i]; cur; cur = next ) {
            next = cur->next;
            REALLOC(cur,0);
        }
        m->array[i] = 0;
    }
    m->count = 0;
}

void (set_free)(set* m) {
    (set_clear)(m);

    array_free(m->array);
    m->array = 0;

    set zero = {0};
    *m = zero;
}

#endif
