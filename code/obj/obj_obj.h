// C objects framework
// - rlyeh, public domain.
//
// ## object limitations
// - 8-byte overhead per object
// - XX-byte overhead per object-entity
// - 32 components max per object-entity
// - 256 classes max per game
// - 256 references max per object
// - 1024K bytes max per object
// - 8 generations + 64K IDs per running instance (19-bit IDs)
// - support for pragma pack(1) structs not enabled by default.

#if !CODE

/* /!\ if you plan to use pragma pack(1) on any struct, you need #define OBJ_MIN_PRAGMAPACK_BITS 0 at the expense of max class size /!\ */
#ifndef   OBJ_MIN_PRAGMAPACK_BITS
//#define OBJ_MIN_PRAGMAPACK_BITS 3 // allows pragma packs >= 8. objsizew becomes 8<<3, so 2048 bytes max per class (default)
#define   OBJ_MIN_PRAGMAPACK_BITS 2 // allows pragma packs >= 4. objsizew becomes 8<<2, so 1024 bytes max per class
//#define OBJ_MIN_PRAGMAPACK_BITS 1 // allows pragma packs >= 2. objsizew becomes 8<<1, so  512 bytes max per class
//#define OBJ_MIN_PRAGMAPACK_BITS 0 // allows pragma packs >= 1. objsizew becomes 8<<0, so  256 bytes max per class
#endif

#define OBJHEADER \
    struct { \
        ifdef(debug, const char *objname;) \
        union { \
            uintptr_t objheader; \
            ifndef(HAS_BITFIELDS,, \
            struct {  \
            uintptr_t objtype:8; \
            uintptr_t objsizew:8; \
            uintptr_t objrefs:8; \
            uintptr_t objheap:1; \
            uintptr_t objcomps:1; /* << can be removed? check payload ptr instead? */ \
            uintptr_t objunused:64-8-8-8-1-1-ID_INDEX_BITS-ID_COUNT_BITS; /*19*/ \
            uintptr_t objid:ID_INDEX_BITS+ID_COUNT_BITS; /*16+3*/ \
            };) \
        }; \
        array(struct obj*) objchildren; \
    };

#ifndef OBJ
#define OBJ \
    OBJHEADER
#endif

// ----------------------------------------------------------------------------
// syntax sugars

#ifdef OBJTYPE
#undef OBJTYPE
#endif

#define OBJTYPE(T) \
    OBJTYPE_##T

#define OBJTYPEDEF(NAME,N) \
     enum { OBJTYPE(NAME) = N }; \
     STATIC_ASSERT( N <= 255 ); \
     STATIC_ASSERT( sizeof(NAME) == ((sizeof(NAME)>>OBJ_MIN_PRAGMAPACK_BITS)<<OBJ_MIN_PRAGMAPACK_BITS) ); // (sizeof(NAME) & ((1<<OBJ_MIN_PRAGMAPACK_BITS)-1)) == 0 );

// ----------------------------------------------------------------------------
// objects

#define TYPEDEF_STRUCT(NAME,N,...) \
    typedef struct NAME { OBJ \
        __VA_ARGS__ \
        char payload[0]; \
    } NAME; OBJTYPEDEF(NAME,N);

// TYPEDEF_STRUCT(obj,0);
    typedef struct obj { OBJ } obj;

// ----------------------------------------------------------------------------
// entities

#define OBJCOMPONENTS_MAX 32
#define OBJCOMPONENTS_ALL_ENABLED 0xAAAAAAAAAAAAAAAAULL
#define OBJCOMPONENTS_ALL_FLAGGED 0x5555555555555555ULL
#define COMPONENTS_ONLY(x) ((x) & ~OBJCOMPONENTS_ALL_FLAGGED)

#ifndef ENTITY
#define ENTITY \
    struct { OBJHEADER union { ifndef(HAS_BITFIELDS,, struct { uintptr_t objenabled:OBJCOMPONENTS_MAX, objflagged:OBJCOMPONENTS_MAX; };) uintptr_t cflags; }; void *c[OBJCOMPONENTS_MAX]; };
#endif

#define TYPEDEF_ENTITY(NAME,N,...) \
    typedef struct NAME { ENTITY \
        __VA_ARGS__ \
        char payload[0]; \
    } NAME; OBJTYPEDEF(NAME,N);

// OBJTYPEDEF(entity,1)
    typedef struct entity { ENTITY } entity;

#define entity_new(TYPE, ...)             OBJ_CTOR(TYPE, #TYPE, 1, 0, __VA_ARGS__)
#define entity_new_ext(TYPE, NAME, ...)   OBJ_CTOR(TYPE,  NAME, 1, 0, __VA_ARGS__)

// ----------------------------------------------------------------------------
// heap/stack ctor/dtor

static __thread obj *objtmp;
#define OBJ_CTOR_HDR(PTR,HEAP,SIZEOF_OBJ,OBJ_TYPE) ( \
        (PTR)->objheader = HEAP ? id_make(PTR) : 0, /*should assign to .objid instead. however, id_make() returns shifted bits already*/ \
        (PTR)->objtype = (OBJ_TYPE), \
        (PTR)->objheap = (HEAP), \
        (PTR)->objsizew = (SIZEOF_OBJ>>OBJ_MIN_PRAGMAPACK_BITS))
#define OBJ_CTOR_PTR(PTR,HEAP,SIZEOF_OBJ,OBJ_TYPE) ( \
        OBJ_CTOR_HDR(PTR,HEAP,SIZEOF_OBJ,OBJ_TYPE), \
        obj_ctor(PTR))
#define OBJ_CTOR(TYPE, NAME, HEAP, PAYLOAD_SIZE, ...) (TYPE*)( \
        objtmp = (HEAP ? MALLOC(sizeof(TYPE)+(PAYLOAD_SIZE)) : ALLOCA(sizeof(TYPE)+(PAYLOAD_SIZE))), \
        *(TYPE*)objtmp = ((TYPE){ {0,}, __VA_ARGS__}), \
        ((PAYLOAD_SIZE) ? memset((char*)objtmp + sizeof(TYPE), 0, (PAYLOAD_SIZE)) : objtmp), \
        ( OBJTYPES[ OBJTYPE(TYPE) ] = #TYPE ), \
        OBJ_CTOR_PTR(objtmp, HEAP,sizeof(TYPE),OBJTYPE(TYPE)), \
        ifdef(debug, (obj_printf)(objtmp, va("%s", trace(+16))), 0), \
        obj_setname(objtmp, NAME))

#define obj(TYPE, ...)                *OBJ_CTOR(TYPE, #TYPE, 0, 0, __VA_ARGS__)
#define obj_new(TYPE, ...)             OBJ_CTOR(TYPE, #TYPE, 1, 0, __VA_ARGS__)
#define obj_new_name(TYPE, NAME, ...)  OBJ_CTOR(TYPE,  NAME, 1, 0, __VA_ARGS__)

void*   obj_malloc(unsigned sz);
void*   obj_free(void *o);

// ----------------------------------------------------------------------------
// obj generics. can be extended.

#define obj_ctor(o,...) obj_method(ctor, o, ##__VA_ARGS__)
#define obj_dtor(o,...) obj_method(dtor, o, ##__VA_ARGS__)

#define obj_save(o,...) obj_method(save, o, ##__VA_ARGS__)
#define obj_load(o,...) obj_method(load, o, ##__VA_ARGS__)

#define obj_test(o,...) obj_method(test, o, ##__VA_ARGS__)

#define obj_init(o,...) obj_method(init, o, ##__VA_ARGS__)
#define obj_quit(o,...) obj_method(quit, o, ##__VA_ARGS__)
#define obj_tick(o,...) obj_method(tick, o, ##__VA_ARGS__)
#define obj_draw(o,...) obj_method(draw, o, ##__VA_ARGS__)

#define obj_lerp(o,...) obj_method(lerp, o, ##__VA_ARGS__)
#define obj_edit(o,...) obj_method(edit, o, ##__VA_ARGS__)
#define obj_menu(o,...) obj_method(menu, o, ##__VA_ARGS__)
#define obj_aabb(o,...) obj_method(aabb, o, ##__VA_ARGS__)
#define obj_icon(o,...) obj_method(icon, o, ##__VA_ARGS__)

// --- syntax sugars

#define obj_extend(T,method)       (obj_##method[OBJTYPE(T)] = (void*)T##_##method)
#define obj_extend_t(T,method)     (obj_##method[OBJTYPE(T##_t)] = (void*)T##_##method)
#define obj_method(method,o,...)   (obj_##method[((struct obj*)(o))->objtype](o,##__VA_ARGS__)) // (obj_##method[((struct obj*)(o))->objtype]((o), ##__VA_ARGS__))
#define obj_hasmethod(o,method)    (obj_typeid(o)[obj_##method])

#define obj_vtable(method,RC,...)   RC MACRO(obj_##method)(){ __VA_ARGS__ }; RC (*obj_##method[256])() = { REPEAT256(MACRO(obj_##method)) };
#define obj_vtable_null(method,RC)  RC (*obj_##method[256])() = { 0 }; // null virtual table. will crash unless obj_extend'ed

#define REPEAT16(f)  f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f ///-
#define REPEAT64(f)  REPEAT16(f),REPEAT16(f),REPEAT16(f),REPEAT16(f) ///-
#define REPEAT256(f) REPEAT64(f),REPEAT64(f),REPEAT64(f),REPEAT64(f) ///-

#undef  EXTEND
#define EXTEND(...) EXPAND(EXTEND, __VA_ARGS__)
#define EXTEND2(o,F1) obj_extend(o,F1) ///-
#define EXTEND3(o,F1,F2) obj_extend(o,F1), obj_extend(o,F2) ///-
#define EXTEND4(o,F1,F2,F3) obj_extend(o,F1), obj_extend(o,F2), obj_extend(o,F3) ///-
#define EXTEND5(o,F1,F2,F3,F4) obj_extend(o,F1), obj_extend(o,F2), obj_extend(o,F3), obj_extend(o,F4) ///-
#define EXTEND6(o,F1,F2,F3,F4,F5) obj_extend(o,F1), obj_extend(o,F2), obj_extend(o,F3), obj_extend(o,F4), obj_extend(o,F5) ///-
#define EXTEND7(o,F1,F2,F3,F4,F5,F6) obj_extend(o,F1), obj_extend(o,F2), obj_extend(o,F3), obj_extend(o,F4), obj_extend(o,F5), obj_extend(o,F6) ///-
#define EXTEND8(o,F1,F2,F3,F4,F5,F6,F7) obj_extend(o,F1), obj_extend(o,F2), obj_extend(o,F3), obj_extend(o,F4), obj_extend(o,F5), obj_extend(o,F6), obj_extend(o,F7) ///-
#define EXTEND9(o,F1,F2,F3,F4,F5,F6,F7,F8) obj_extend(o,F1), obj_extend(o,F2), obj_extend(o,F3), obj_extend(o,F4), obj_extend(o,F5), obj_extend(o,F6), obj_extend(o,F7), obj_extend(o,F8) ///-
#define EXTEND10(o,F1,F2,F3,F4,F5,F6,F7,F8,F9) obj_extend(o,F1), obj_extend(o,F2), obj_extend(o,F3), obj_extend(o,F4), obj_extend(o,F5), obj_extend(o,F6), obj_extend(o,F7), obj_extend(o,F8), obj_extend(o,F9) ///-

#define EXTEND_T(...) EXPAND(EXTEND_T, __VA_ARGS__)
#define EXTEND_T2(o,F1) obj_extend_t(o,F1) ///-
#define EXTEND_T3(o,F1,F2) obj_extend_t(o,F1), obj_extend_t(o,F2) ///-
#define EXTEND_T4(o,F1,F2,F3) obj_extend_t(o,F1), obj_extend_t(o,F2), obj_extend_t(o,F3) ///-
#define EXTEND_T5(o,F1,F2,F3,F4) obj_extend_t(o,F1), obj_extend_t(o,F2), obj_extend_t(o,F3), obj_extend_t(o,F4) ///-
#define EXTEND_T6(o,F1,F2,F3,F4,F5) obj_extend_t(o,F1), obj_extend_t(o,F2), obj_extend_t(o,F3), obj_extend_t(o,F4), obj_extend_t(o,F5) ///-
#define EXTEND_T7(o,F1,F2,F3,F4,F5,F6) obj_extend_t(o,F1), obj_extend_t(o,F2), obj_extend_t(o,F3), obj_extend_t(o,F4), obj_extend_t(o,F5), obj_extend_t(o,F6) ///-
#define EXTEND_T8(o,F1,F2,F3,F4,F5,F6,F7) obj_extend_t(o,F1), obj_extend_t(o,F2), obj_extend_t(o,F3), obj_extend_t(o,F4), obj_extend_t(o,F5), obj_extend_t(o,F6), obj_extend_t(o,F7) ///-
#define EXTEND_T9(o,F1,F2,F3,F4,F5,F6,F7,F8) obj_extend_t(o,F1), obj_extend_t(o,F2), obj_extend_t(o,F3), obj_extend_t(o,F4), obj_extend_t(o,F5), obj_extend_t(o,F6), obj_extend_t(o,F7), obj_extend_t(o,F8) ///-
#define EXTEND_T10(o,F1,F2,F3,F4,F5,F6,F7,F8,F9) obj_extend_t(o,F1), obj_extend_t(o,F2), obj_extend_t(o,F3), obj_extend_t(o,F4), obj_extend_t(o,F5), obj_extend_t(o,F6), obj_extend_t(o,F7), obj_extend_t(o,F8), obj_extend_t(o,F9) ///-

// --- declare vtables

API extern void  (*obj_ctor[256])(); ///-
API extern void  (*obj_dtor[256])(); ///-

API extern char* (*obj_save[256])(); ///-
API extern bool  (*obj_load[256])(); ///-
API extern int   (*obj_test[256])(); ///-

API extern int   (*obj_init[256])(); ///-
API extern int   (*obj_quit[256])(); ///-
API extern int   (*obj_tick[256])(); ///-
API extern int   (*obj_draw[256])(); ///-
API extern int   (*obj_lerp[256])(); ///-

API extern int   (*obj_aabb[256])(); ///-
API extern int   (*obj_edit[256])(); ///-
API extern int   (*obj_menu[256])(); ///-
API extern char* (*obj_icon[256])(); ///-

API extern const char*OBJTYPES[256]; ///-

// ----------------------------------------------------------------------------
// core

API uintptr_t   obj_header(const void *o);
API uintptr_t   obj_id(const void *o);

API const char* obj_type(const void *o);
API unsigned    obj_typeid(const void *o);

API int         obj_sizeof(const void *o);
API int         obj_size(const void *o); // size of all members together in struct. may include padding bytes.

API char*       obj_data(void *o); // pointer to the first member in struct
API const char* obj_datac(const void *o); // const pointer to the first struct member

API void*       obj_payload(const void *o); // pointer right after last member in struct
API void*       obj_zero(void *o); // reset all object members

// ----------------------------------------------------------------------------
// refcounting

API void*       obj_ref(void *oo);
API void*       obj_unref(void *oo);

// ----------------------------------------------------------------------------
// scene tree

#define each_objchild(p,T,o) /*non-recursive*/ \
    (array(struct obj*)* children = obj_children(p); children; children = 0) \
        for(int _i = 1, _end = array_count(*children); _i < _end; ++_i) \
            for(T o = (T)((*children)[_i]); o && (obj_parent(o) == p); o = 0)

API obj*        obj_detach(void *c);
API obj*        obj_attach(void *o, void *c);

API obj*        obj_root(const void *o);
API obj*        obj_parent(const void *o);
API array(obj*)*obj_children(const void *o); // child[0]: parent, child[1]: 1st child, child[2]: 2nd child...
API array(obj*)*obj_siblings(const void *o); // child[0]: grandpa, child[1]: sibling1, child[2]: sibling2...

API int         obj_dumptree(const void *o);

// ----------------------------------------------------------------------------
// metadata

API void*       obj_setmeta(void *o, const char *key, const char *value);
API const char* obj_meta(const void *o, const char *key);

API void*       obj_setname(void *o, const char *name);
API const char* obj_name(const void *o);

// ----------------------------------------------------------------------------
// stl

API void*       obj_swap(void *dst, void *src);
API void*       obj_copy_fast(void *dst, const void *src);
API void*       obj_copy(void *dst, const void *src);

API int         obj_comp_fast(const void *a, const void *b);
API int         obj_comp(const void *a, const void *b);
API int         obj_lesser(const void *a, const void *b);
API int         obj_greater(const void *a, const void *b);
API int         obj_equal(const void *a, const void *b);

API uint64_t    obj_hash(const void *o);

// ----------------------------------------------------------------------------
// debug

API bool        obj_hexdump(const void *oo);
API int         obj_print(const void *o);

API int         obj_printf(const void *o, const char *text);
API int         obj_console(const void *o); // obj_output() ?

#define         obj_printf(o, ...) obj_printf(o, va(__VA_ARGS__))

// ----------------------------------------------------------------------------
// serialization

API char*       obj_saveini(const void *o);
API obj*        obj_mergeini(void *o, const char *ini);
API obj*        obj_loadini(void *o, const char *ini);

API char*       obj_savejson(const void *o);
API obj*        obj_mergejson(void *o, const char *json);
API obj*        obj_loadjson(void *o, const char *json);

API char*       obj_savebin(const void *o);
API obj*        obj_mergebin(void *o, const char *sav);
API obj*        obj_loadbin(void *o, const char *sav);

API char*       obj_savempack(const void *o); // @todo
API obj*        obj_mergempack(void *o, const char *sav); // @todo
API obj*        obj_loadmpack(void *o, const char *sav); // @todo

API int         obj_push(const void *o);
API int         obj_pop(void *o);

// ----------------------------------------------------------------------------
// components

API bool        obj_addcomponent(entity *e, unsigned c, void *ptr);
API bool        obj_hascomponent(entity *e, unsigned c);
API void*       obj_getcomponent(entity *e, unsigned c);
API bool        obj_delcomponent(entity *e, unsigned c);
API bool        obj_usecomponent(entity *e, unsigned c);
API bool        obj_offcomponent(entity *e, unsigned c);

API char*       entity_save(entity *self);

// ----------------------------------------------------------------------------
// reflection

#define each_objmember(oo,TYPE,NAME,PTR) \
    (array(reflect_t) *found_ = members_find(obj_type(oo)); found_; found_ = 0) \
        for(int it_ = 0, end_ = array_count(*found_); it_ != end_; ++it_ ) \
            for(reflect_t *R = &(*found_)[it_]; R; R = 0 ) \
                for(const char *NAME = R->name, *TYPE = R->type; NAME || TYPE; ) \
                    for(void *PTR = ((char*)oo) + R->sz ; NAME || TYPE ; NAME = TYPE = 0 )

API void*       obj_clone(const void *src);
API void*       obj_merge(void *dst, const void *src); // @testme
API void*       obj_mutate(void *dst, const void *src);
API void*       obj_make(const char *str);

#else

// ----------------------------------------------------------------------
// C objects framework
// - rlyeh, public domain.

// --- implement new vtables

obj_vtable(ctor, void,  {} );
obj_vtable(dtor, void,  {} );

obj_vtable_null(save, char* );
obj_vtable_null(load, bool  );
obj_vtable_null(test, int   );

obj_vtable_null(init, int   );
obj_vtable_null(quit, int   );
obj_vtable_null(tick, int   );
obj_vtable_null(draw, int   );

obj_vtable_null(lerp, int   );
obj_vtable_null(edit, int   ); // OSC cmds: argc,argv "undo","redo","cut","copy","paste","edit","view","menu"
obj_vtable_null(menu, int   );
obj_vtable_null(aabb, int   );
obj_vtable_null(icon, char* );

// ----------------------------------------------------------------------------

const char *OBJTYPES[256] = { 0 }; // = { REPEAT256("") };

// ----------------------------------------------------------------------------
// heap/stack ctor/dtor

void *obj_malloc(unsigned sz) {
    //sz = sizeof(obj) + sz + sizeof(array(obj*))); // useful?
    obj *ptr = CALLOC(1, sz);
    OBJ_CTOR_HDR(ptr,1,sz,OBJTYPE_obj);
    return ptr;
}
void *obj_free(void *o) {
    if( !((obj*)o)->objrefs ) {
        obj_detach(o);
        obj_dtor(o);
        //obj_zero(o);
        if( ((obj*)o)->objheap ) {
            FREE(o);
        }
        return 0;
    }
    return o; // cannot destroy: object is still referenced
}

// ----------------------------------------------------------------------------
// core

uintptr_t obj_header(const void *o) {
    return ((obj*)o)->objheader;
}
uintptr_t obj_id(const void *o) {
    return ((obj*)o)->objid;
}
unsigned obj_typeid(const void *o) {
    return ((obj*)o)->objtype;
}
const char *obj_type(const void *o) {
    return OBJTYPES[ (((obj*)o)->objtype) ];
}
//const char *obj_name(const void *o) {
//    return quark(((obj*)o)->objnameid);
//}
int obj_sizeof(const void *o) {
    return (int)( ((const obj*)o)->objsizew << OBJ_MIN_PRAGMAPACK_BITS );
}
int obj_size(const void *o) { // size of all members together in struct. may include padding bytes.
    static int obj_sizes[256] = {0};
    unsigned objtypeid = ((obj*)o)->objtype;
    if( objtypeid > 1 && !obj_sizes[objtypeid] ) { // check reflection for a more accurate objsize (without padding bits)
        reflect_init();
        array(reflect_t) *found = map_find(members, intern(obj_type(o)));
        if(!found)
            obj_sizes[objtypeid] = obj_sizeof(o) - sizeof(obj); // @fixme: -= sizeof(entity);
        else
        for each_array_ptr(*found, reflect_t, it)
            obj_sizes[objtypeid] += it->bytes;
    }
    return obj_sizes[objtypeid];
}
char *obj_data(void *o) { // pointer to the first member in struct
    return (char*)o + sizeof(obj);
}
const char *obj_datac(const void *o) { // const pointer to the first struct member
    return (const char*)o + sizeof(obj);
}
void* obj_payload(const void *o) { // pointer right after last member in struct
    return (char*)o + (((obj*)o)->objsizew<<OBJ_MIN_PRAGMAPACK_BITS);
}
void *obj_zero(void *o) { // reset all object members
    return memset(obj_data(o), 0, obj_size(o)), o;
}

static
void test_obj_core() {
    obj *r = obj_new_name(obj, "root");
    obj *s = obj_new_name(obj, "root");

    test(r);
    test( 0 == strcmp(obj_type(r), "obj") );
    test( 0 == strcmp(obj_name(r), "root") );
    test( OBJTYPE_obj == obj_typeid(r) );

    test(s);
    test( 0 == strcmp(obj_type(s), "obj") );
    test( 0 == strcmp(obj_name(s), "root") );
    test( OBJTYPE_obj == obj_typeid(s) );

    test( obj_id(r) != 0 );
    test( obj_id(s) != 0 );
    test( obj_id(r) != obj_id(s) );

    obj t = obj(obj); obj_setname(&t, "root");
    obj u = obj(obj); obj_setname(&u, "root");

    test(&t);
    test( 0 == strcmp(obj_type(&t), "obj") );
    test( 0 == strcmp(obj_name(&t), "root") );
    test( OBJTYPE_obj == obj_typeid(&t) );

    test(&u);
    test( 0 == strcmp(obj_type(&u), "obj") );
    test( 0 == strcmp(obj_name(&u), "root") );
    test( OBJTYPE_obj == obj_typeid(&u) );

    test( obj_id(&t) == 0 );
    test( obj_id(&u) == 0 );
    test( obj_id(&t) == obj_id(&u) );
}

// ----------------------------------------------------------------------------
// refcounting

// static int __thread global_ref_count; // @fixme: make it atomic
// static void objref_check_atexit(void) {
//     if(global_ref_count) fprintf(stderr, ANSI_YELLOW("Warn! obj_refs not zero (%d)\n"), global_ref_count);
// }
// AUTORUN { (atexit)(objref_check_atexit); }

void *obj_ref(void *oo) {
    obj* o = (obj*)oo;
    int num = o->objrefs;
    ++o->objrefs;
    assert( num < o->objrefs && "Object referenced too many times");
    //++global_ref_count;
    return o;
}
void *obj_unref(void *oo) {
    obj* o = (obj*)oo;
    if( o->objrefs ) --o->objrefs;
    if( o->objrefs ) return o;
    obj_free(o);
    //--global_ref_count;
    return 0;
}

// ----------------------------------------------------------------------------
// scene tree

array(obj*)* obj_children(const void *o) {
    array(obj*) *c = &((obj*)o)->objchildren;
    if(!(*c)) array_push((*c), NULL); // default parenting: none. @todo: optimize & move this at construction time
    return c;
}
obj* obj_parent(const void *o) {
    array(obj*) *c = obj_children(o);
    return 0[*c]; // (*c) ? 0[*c] : NULL;
}
obj* obj_root(const void *o) {
    while( obj_parent(o) ) o = obj_parent(o);
    return (obj*)o;
}
array(obj*)* obj_siblings(const void *o) {
    return obj_children(obj_parent(o));
}

static
obj* obj_reparent(obj *o, const void *p) {
    array(obj*) *c = obj_children(o);
    0[*c] = (void*)p;
    return o;
}

obj* obj_detach(void *c) {
    obj *p = obj_parent(c);
    if( p ) {
        uintptr_t id = obj_id(c);

        array(obj*) *oo = obj_children(p);
        for( int i = 1, end = array_count(*oo); i < end; ++i) {
            obj *v = (*oo)[i];
            {
                if( obj_id(v) == id ) {
                    obj_reparent(c, 0);
                    array_erase_slow(*oo, i);
                    return c;
                }
            }
        }
    }
    return 0;
}
obj* obj_attach(void *o, void *c) {
    // reattach
    obj_detach(c);
    obj_reparent(c, o);
    // insert into children
    array(obj*) *p = obj_children(o);
    array_push(*p, c);
    return o;
}

int obj_dumptree(const void *o) {
    static int tabs = 0;
    printf("%*s" "+- %s\n", tabs++, "", obj_name(o));
    for each_objchild(o, obj*, v) {
        obj_dumptree(v);
    }
    --tabs;
    return 0;
}

static
void test_obj_scene() {
    obj *r = obj_new_name(obj, "root");           // root
    obj *c1 = obj_new_name(obj, "child1");        // child1
    obj *c2 = obj_new_name(obj, "child2");        // child2
    obj *gc1 = obj_new_name(obj, "grandchild1");  // grandchild1
    obj *gc2 = obj_new_name(obj, "grandchild2");  // grandchild2
    obj *gc3 = obj_new_name(obj, "grandchild3");  // grandchild3

    test( !obj_parent(r) );
    test( !obj_parent(c1) );
    test( !obj_parent(c2) );
    test( !obj_parent(gc1) );
    test( !obj_parent(gc2) );
    test( !obj_parent(gc3) );
    test( obj_root(r) == r );
    test( obj_root(c1) == c1 );
    test( obj_root(c2) == c2 );
    test( obj_root(gc1) == gc1 );
    test( obj_root(gc2) == gc2 );
    test( obj_root(gc3) == gc3 );

                               // r
    obj_attach(r, c1);         // +- c1
    obj_attach(c1, gc1);       //  +- gc1
    obj_attach(r, c2);         // +- c2
    obj_attach(c2, gc2);       //  +- gc2
    obj_attach(c2, gc3);       //  +- gc3

    obj_dumptree(r);
    // puts("---");

    test( obj_parent(r) == 0 );
    test( obj_parent(c1) == r );
    test( obj_parent(c2) == r );
    test( obj_parent(gc1) == c1 );
    test( obj_parent(gc2) == c2 );
    test( obj_parent(gc3) == c2 );

    test( obj_root(r) == r );
    test( obj_root(c1) == r );
    test( obj_root(c2) == r );
    test( obj_root(gc1) == r );
    test( obj_root(gc2) == r );
    test( obj_root(gc3) == r );

    for each_objchild(r, obj*, o) test( o == c1 || o == c2 );
    for each_objchild(c1, obj*, o) test( o == gc1 );
    for each_objchild(c2, obj*, o) test( o == gc2 || o == gc3 );

    obj_detach(c1);
    test( !obj_parent(c1) );
    for each_objchild(r, obj*, o) test( o != c1 );
    for each_objchild(c1, obj*, o) test( o == gc1 );

    obj_detach(c2);
    test( !obj_parent(c2) );
    for each_objchild(r, obj*, o) test( o != c2 );
    for each_objchild(c2, obj*, o) test( o == gc2 || o == gc3 );
}

// ----------------------------------------------------------------------------
// metadata

static map(int,int) oms;
#if HAS_LOCK
static thread_mutex_t *oms_lock;
#endif
void *obj_setmeta(void *o, const char *key, const char *value) {
    void *ret = 0;
    #if HAS_LOCK
    do_threadlock(oms_lock)
    #endif
    {
        if(!oms) map_init_int(oms);
        int *q = map_find_or_add(oms, intern(va("%p-%s",(void*)obj_id((obj*)o),key)), 0);
        if(!*q && !value[0]) {} else *q = intern(value);
        quark(*q), ret = o;
    }
    return ret;
}
const char* obj_meta(const void *o, const char *key) {
    const char *ret = 0;
    #if HAS_LOCK
    do_threadlock(oms_lock)
    #endif
    {
        if(!oms) map_init_int(oms);
        int *q = map_find_or_add(oms, intern(va("%p-%s",(void*)obj_id((obj*)o),key)), 0);
        ret = quark(*q);
    }
    return ret;
}

void *obj_setname(void *o, const char *name) {
    ifdef(debug,((obj*)o)->objname = name);
    return obj_setmeta(o, "name", name);
}
const char *obj_name(const void *o) {
    const char *objname = obj_meta(o, "name");
    return objname[0] ? objname : "obj";
}


static
void test_obj_metadatas( void *o1 ) {
    obj *o = (obj *)o1;
    test( !strcmp("", obj_meta(o, "has_passed_test")) );
    test( obj_setmeta(o, "has_passed_test", "yes") );
    test( !strcmp("yes", obj_meta(o, "has_passed_test")) );
}

// ----------------------------------------------------------------------------
// stl

void* obj_swap(void *dst, void *src) { // @testme
    int len = obj_size(dst);
    char *buffer = ALLOCA(len);
    memcpy(buffer,        obj_datac(dst), len);
    memcpy(obj_data(dst), obj_datac(src), len);
    memcpy(obj_data(src), buffer,         len);
    return dst;
}

void* obj_copy_fast(void *dst, const void *src) {
    // note: prefer obj_copy() as it should handle pointers and guids as well
    return memcpy(obj_data(dst), obj_datac(src), obj_size(dst));
}
void* obj_copy(void *dst, const void *src) { // @testme
    // @todo: use obj_copy_fast() silently if the object does not contain any pointers/guids
    return obj_loadini(dst, obj_saveini(src));
    // return obj_load(dst, obj_save(src));
    // return obj_loadbin(dst, obj_savebin(src));
    // return obj_loadini(dst, obj_saveini(src));
    // return obj_loadjson(dst, obj_savejson(src));
    // return obj_loadmpack(dst, obj_savempack(src));
}

int obj_comp_fast(const void *a, const void *b) {
    // note: prefer obj_comp() as it should handle pointers and guids as well
    return memcmp(obj_datac(a), obj_datac(b), obj_size(a));
}
int obj_comp(const void *a, const void *b) {
    // @todo: use obj_comp_fast() silently if the object does not contain any pointers/guids
    return strcmp(obj_saveini(a),obj_saveini(b));
}
int obj_lesser(const void *a, const void *b) {
    return obj_comp(a,b) < 0;
}
int obj_greater(const void *a, const void *b) {
    return obj_comp(a,b) > 0;
}
int obj_equal(const void *a, const void *b) {
    return obj_comp(a,b) == 0;
}

uint64_t obj_hash(const void *o) {
    return hash_bin(obj_datac(o), obj_size(o));
}

static
void test_obj_similarity(void *o1, void *o2) {
    obj *b = (obj*)o1;
    obj *c = (obj*)o2;
    test( 0 == strcmp(obj_name(b),obj_name(c)) );
    test( 0 == strcmp(obj_type(b),obj_type(c)) );
}
static
void test_obj_equality(void *o1, void *o2) {
    obj *b = (obj*)o1;
    obj *c = (obj*)o2;
    test_obj_similarity(b, c);
    test( obj_size(b) == obj_size(c) );
    test( obj_hash(b) == obj_hash(c) );
    test( 0 == obj_comp(b,c) );
    test( obj_equal(b,c) );
    test( !obj_lesser(b,c) );
    test( !obj_greater(b,c) );
}
static
void test_obj_exact(void *o1, void *o2) {
    obj *b = (obj*)o1;
    obj *c = (obj*)o2;
    test_obj_equality(b, c);
    test( obj_header(b) == obj_header(c) );
    test( 0 == memcmp(b, c, obj_sizeof(b)) );
}

// ----------------------------------------------------------------------------
// debug

bool obj_hexdump(const void *oo) {
    const obj *o = (const obj *)oo;
    int header = 1 * sizeof(obj);
    printf("; name[%s] type[%s] id[%d..%d] unused[%08x] sizeof[%02d] %p\n",
        obj_name(o), obj_type(o),
        (int)o->objid>>16, (int)o->objid&0xffff, (int)o->objunused,
        obj_sizeof(o), (void*)o->objheader);
    return hexdump(obj_datac(o) - header, obj_size(o) + header), 1;
}
int obj_print(const void *o) {
    char *sav = obj_saveini(o); // obj_savejson(o)
    return puts(sav);
}
static char *obj_tempname = 0;
static FILE *obj_filelog = 0;
int (obj_printf)(const void *o, const char *text) {
    if( !obj_tempname ) {
        obj_tempname = stringf("%s.log", app_name());
        unlink(obj_tempname);
        obj_filelog = fopen(obj_tempname, "w+b");
        if( obj_filelog ) fseek(obj_filelog, 0L, SEEK_SET);
    }
    int rc = 0;
    for( char *end; (end = strchr(text, '\n')) != NULL; ) {
        rc |= fprintf(obj_filelog, "[%p] %.*s\n", o, (int)(end - text), text );
        text = end + 1;
    }
    if( text[0] ) rc |= fprintf(obj_filelog, "[%p] %s\n", o, text);
    return rc;
}
int obj_console(const void *o) { // obj_output() ?
    if( obj_filelog ) fflush(obj_filelog);
    return obj_tempname && !system(va(ifdef(win32,"type \"%s\" | find \"[%p]\"", "cat %s | grep \"[%p]\""), obj_tempname, o));
}

static
void test_obj_console(void *o1) {
    obj *o = (obj *)o1;

    obj_printf(o, "this is [%s], line 1\n", obj_name(o));
    obj_printf(NULL, "this line does not belong to any object\n");
    obj_printf(o, "this is [%s], line 2\n", obj_name(o));
    obj_console(o);
}

// ----------------------------------------------------------------------------
// serialization

const char *p2s(const char *type, void *p) {
    // @todo: p2s(int interned_type, void *p)
    /**/ if( !strcmp(type, "int") ) return itoa1(*(int*)p);
    else if( !strcmp(type, "unsigned") ) return itoa1(*(unsigned*)p);
    else if( !strcmp(type, "float") ) return ftoa1(*(float*)p);
    else if( !strcmp(type, "double") ) return ftoa1(*(double*)p);
    else if( !strcmp(type, "uintptr_t") ) return va("%p", (void*)*(uintptr_t*)p);
    else if( !strcmp(type, "vec2i") ) return itoa2(*(vec2i*)p);
    else if( !strcmp(type, "vec3i") ) return itoa3(*(vec3i*)p);
    else if( !strcmp(type, "vec2") ) return ftoa2(*(vec2*)p);
    else if( !strcmp(type, "vec3") ) return ftoa3(*(vec3*)p);
    else if( !strcmp(type, "vec4") ) return ftoa4(*(vec4*)p);
    else if( !strcmp(type, "rgb") ) return rgbatoa(*(unsigned*)p);
    else if( !strcmp(type, "rgba") ) return rgbatoa(*(unsigned*)p);
    else if( !strcmp(type, "char*") || !strcmp(type, "string") ) return va("%s", *(char**)p);
    // @todo: if strchr('*') assume obj, if reflected save guid: obj_id();
    return printf(ANSI_YELLOW("p2s: cannot serialize `%s` type\n"), type), "";
}
bool s2p(void *P, const char *type, const char *str) {
    int i; unsigned u; float f; double g; char *s = 0; uintptr_t p;
    vec2 v2; vec3 v3; vec4 v4; vec2i v2i; vec3i v3i;
    /**/ if( !strcmp(type, "int") )       return !!memcpy(P, (i = atoi1(str), &i), sizeof(i));
    else if( !strcmp(type, "unsigned") )  return !!memcpy(P, (u = atoi1(str), &u), sizeof(u));
    else if( !strcmp(type, "vec2i") )     return !!memcpy(P, (v2i = atoi2(str), &v2i), sizeof(v2i));
    else if( !strcmp(type, "vec3i") )     return !!memcpy(P, (v3i = atoi3(str), &v3i), sizeof(v3i));
    else if( !strcmp(type, "float") )     return !!memcpy(P, (f = atof1(str), &f), sizeof(f));
    else if( !strcmp(type, "double") )    return !!memcpy(P, (g = atof1(str), &g), sizeof(g));
    else if( !strcmp(type, "vec2") )      return !!memcpy(P, (v2 = atof2(str), &v2), sizeof(v2));
    else if( !strcmp(type, "vec3") )      return !!memcpy(P, (v3 = atof3(str), &v3), sizeof(v3));
    else if( !strcmp(type, "vec4") )      return !!memcpy(P, (v4 = atof4(str), &v4), sizeof(v4));
    else if( !strcmp(type, "rgb") )       return !!memcpy(P, (u = atorgba(str), &u), sizeof(u));
    else if( !strcmp(type, "rgba") )      return !!memcpy(P, (u = atorgba(str), &u), sizeof(u));
    else if( !strcmp(type, "uintptr_t") ) return !!memcpy(P, (p = strtol(str, NULL, 16), &p), sizeof(p));
    else if( !strcmp(type, "char*") || !strcmp(type, "string") ) {
        char substring[128] = {0};
        sscanf(str, "%[^\r\n]", substring);

        strcatf(&s, "%s", substring);

        *(uintptr_t*)(P) = (uintptr_t)s;
        return 1;
    }
    // @todo: if strchr('*') assume obj, if reflected load guid: obj_id();
    return printf(ANSI_YELLOW("s2p: cannot deserialize `%s` type\n"), type), 0;
}

char *obj_saveini(const void *o) { // @testme
    char *out = 0;
    const char *T = obj_type(o);
    strcatf(&out, "[%s] ; v100\n", T);
    for each_member(T,R) {
        const char *sav = p2s(R->type,(char*)(o)+R->sz);
        if(!sav) return FREE(out), NULL;
        strcatf(&out,"%s.%s=%s\n", R->type,R->name,sav );
    }
    char *cpy = va("%s", out);
    FREE(out);
    return cpy;
}
obj *obj_mergeini(void *o, const char *ini) { // @testme
    const char *sqr = strchr(ini, '[');
    if( !sqr ) return 0;
    ini = sqr+1;

    char T[64] = {0};
    if( sscanf(ini, "%63[^]]", T) != 1 ) return 0; // @todo: parse version as well
    ini += strlen(T);

    for each_member(T,R) {
        char *lookup = va("\n%s.%s=", R->type,R->name), *found = 0;

        // type needed? /*
        if(!found) { found = strstr(ini, lookup); if (found) found += strlen(lookup); }
        if(!found) { *lookup = '\r'; }
        if(!found) { found = strstr(ini, lookup); if (found) found += strlen(lookup); }
        // */

        if(!found) lookup = va("\n%s=", R->name);

        if(!found) { found = strstr(ini, lookup); if (found) found += strlen(lookup); }
        if(!found) { *lookup = '\r'; }
        if(!found) { found = strstr(ini, lookup); if (found) found += strlen(lookup); }

        if( found) {
            if(!s2p((char*)(o)+R->sz, R->type, found))
                return 0;
        }
    }
    return o;
}
obj *obj_loadini(void *o, const char *ini) { // @testme
    return obj_mergeini(obj_zero(o), ini);
}

char *obj_savejson(const void *o) {
    char *j = 0;
    const char *T = obj_type(o);
    for each_member(T,R) {
        const char *sav = p2s(R->type,(char*)(o)+R->sz);
        if(!sav) return FREE(j), NULL;
        char is_string = !strcmp(R->type,"char*") || !strcmp(R->type,"string");
        strcatf(&j," %s: %s%s%s,\n", R->name,is_string?"\"":"",sav,is_string?"\"":"" );
    }
    char *out = va("%s: { // v100\n%s}\n", T,j);
    FREE(j);
#if is(debug)
    json5 root = { 0 };
    char *error = json5_parse(&root, va("%s", out), 0);
    assert( !error );
    json5_free(&root);
#endif
    return out;
}
obj *obj_mergejson(void *o, const char *json) {
    // @fixme: va() call below could be optimized out since we could figure it out if json was internally provided (via va or strdup), or user-provided
    json5 root = { 0 };
    char *error = json5_parse(&root, va("%s", json), 0); // @todo: parse version comment
    if( !error && root.type == JSON5_OBJECT && root.count == 1 ) {
        json5 *n = &root.nodes[0];
        char *T = n->name;
        for each_member(T,R) {
            for( int i = 0; i < n->count; ++i ) {
                if( !strcmp(R->name, n->nodes[i].name) ) {
                    void *p = (char*)o + R->sz;
                    /**/ if( n->nodes[i].type == JSON5_UNDEFINED ) {}
                    else if( n->nodes[i].type == JSON5_NULL ) {
                        *(uintptr_t*)(p) = (uintptr_t)0;
                    }
                    else if( n->nodes[i].type == JSON5_BOOL ) {
                        *(bool*)p = n->nodes[i].boolean;
                    }
                    else if( n->nodes[i].type == JSON5_INTEGER ) {
                        if( strstr(R->type, "64" ) )
                        *(int64_t*)p = n->nodes[i].integer;
                        else
                        *(int*)p = n->nodes[i].integer;
                    }
                    else if( n->nodes[i].type == JSON5_STRING ) {
                        char *s = 0;
                        strcatf(&s, "%s", n->nodes[i].string);
                        *(uintptr_t*)(p) = (uintptr_t)s;
                    }
                    else if( n->nodes[i].type == JSON5_REAL ) {
                        if( R->type[0] == 'f' )
                        *(float*)(p) = n->nodes[i].real;
                        else
                        *(double*)(p) = n->nodes[i].real;
                    }
                    else if( n->nodes[i].type == JSON5_OBJECT ) {}
                    else if( n->nodes[i].type == JSON5_ARRAY ) {}
                    break;
                }
            }
        }
    }
    json5_free(&root);
    return error ? 0 : o;
}
obj *obj_loadjson(void *o, const char *json) { // @testme
    return obj_mergejson(obj_zero(o), json);
}

char *obj_savebin(const void *o) { // PACKMSG("ss", "entity_v1", quark(self->objnameid)); // = PACKMSG("p", obj_data(&b), (uint64_t)obj_size(&b));
    int len = cobs_bounds(obj_size(o));
    char *sav = va("%*.s", len, "");
    len = cobs_encode(obj_datac(o), obj_size(o), sav, len);
    sav[len] = '\0';
    return sav;
}
obj *obj_mergebin(void *o, const char *sav) { // UNPACKMSG(sav, "p", obj_data(c), (uint64_t)obj_size(c));
    int outlen = cobs_decode(sav, strlen(sav), obj_data(o), obj_size(o));
    return outlen != obj_size(o) ? NULL : o;
}
obj *obj_loadbin(void *o, const char *sav) {
    return obj_mergebin(obj_zero(o), sav);
}

char *obj_savempack(const void *o) { // @todo
    return "";
}
obj *obj_mergempack(void *o, const char *sav) { // @todo
    return 0;
}
obj *obj_loadmpack(void *o, const char *sav) { // @todo
    return obj_mergempack(obj_zero(o), sav);
}

static __thread map(void*,array(char*)) obj_stack;
int obj_push(const void *o) {
    if(!obj_stack) map_init_ptr(obj_stack);
    array(char*) *found = map_find_or_add(obj_stack,(void*)o,0);

    char *bin = STRDUP(obj_saveini(o)); // @todo: savebin
    array_push(*found, bin);
    return 1;
}
int obj_pop(void *o) {
    if(!obj_stack) map_init_ptr(obj_stack);
    array(char*) *found = map_find_or_add(obj_stack,(void*)o,0);

    char **bin = array_back(*found);
    if( bin ) {
        int rc = !!obj_loadini(o, *bin); // @todo: loadbin
        if( array_count(*found) > 1 ) {
            FREE(*bin);
            array_pop(*found);
        }
        return rc;
    }
    return 0;
}

static
void test_obj_serialization(void *o1, void *o2) {
    obj* b = (obj*)o1;
    obj* c = (obj*)o2;

    char *json = obj_savejson(b); // puts(json);
    test( json[0] );
    char *ini = obj_saveini(b); // puts(ini);
    test( ini[0] );
    char *bin = obj_savebin(b); // puts(bin);
    test( bin[0] );

    obj_push(c);

        test( obj_copy(c,b) );
        test( obj_comp(b,c) == 0 ) || obj_hexdump(b) & obj_hexdump(c);

        test( obj_zero(c) );
        test( obj_comp(c,b) != 0 ) || obj_hexdump(c);
        test( obj_loadbin(c, bin) );
        test( obj_comp(c,b) == 0 ) || obj_hexdump(c) & obj_hexdump(b);

        test( obj_zero(c) );
        test( obj_comp(c,b) != 0 ) || obj_hexdump(c);
        test( obj_loadini(c, ini) );
        test( obj_comp(c,b) == 0 ) || obj_hexdump(c) & obj_hexdump(b);

        test( obj_zero(c) );
        test( obj_comp(c,b) != 0 ) || obj_hexdump(c);
        test( obj_loadjson(c, json) );
        test( obj_comp(c,b) == 0 ) || obj_hexdump(c) & obj_hexdump(b);

    obj_pop(c);
    obj_hexdump(c);
}

// ----------------------------------------------------------------------------
// components

bool obj_addcomponent(entity *e, unsigned c, void *ptr) {
    e->cflags |= (3ULL << c);
    e->c[c & (OBJCOMPONENTS_MAX-1)] = ptr;
    return 1;
}
bool obj_hascomponent(entity *e, unsigned c) {
    return !!(e->cflags & (3ULL << c));
}
void* obj_getcomponent(entity *e, unsigned c) {
    return e->c[c & (OBJCOMPONENTS_MAX-1)];
}
bool obj_delcomponent(entity *e, unsigned c) {
    e->cflags &= ~(3ULL << c);
    e->c[c & (OBJCOMPONENTS_MAX-1)] = NULL;
    return 1;
}
bool obj_usecomponent(entity *e, unsigned c) {
    e->cflags |= (1ULL << c);
    return 1;
}
bool obj_offcomponent(entity *e, unsigned c) {
    e->cflags &= ~(1ULL << c);
    return 0;
}

char *entity_save(entity *self) {
    char *sav = obj_saveini(self);
    return sav;
}

static
void entity_register() {
    do_once {
        STRUCT(entity, uintptr_t, cflags);
        obj_extend(entity, save);
    }
}

AUTORUN{
    entity_register();
}

static
void test_obj_ecs() {
    entity_register(); // why is this required here? autorun init fiasco?

    entity *e = entity_new(entity);
    puts(obj_save(e));

    for( int i = 0; i < 32; ++i) test(0 == obj_hascomponent(e, i));
    for( int i = 0; i < 32; ++i) test(1 == obj_addcomponent(e, i, NULL));
    for( int i = 0; i < 32; ++i) test(1 == obj_hascomponent(e, i));
    for( int i = 0; i < 32; ++i) test(1 == obj_delcomponent(e, i));
    for( int i = 0; i < 32; ++i) test(0 == obj_hascomponent(e, i));
}

// ----------------------------------------------------------------------------
// reflection

void* obj_mutate(void *dst, const void *src) {
    ((obj*)dst)->objheader = ((const obj *)src)->objheader;

#if 0
    // mutate a class. ie, convert a given object class into a different one,
    // while preserving the original metas, components and references as much as possible.
    // @todo iterate per field

    dtor(dst);

        unsigned src_sz = obj_sizeof(src);
        unsigned src_id = obj_id(src);

        void *dst_ptr = *((void**)dst - 1);
        unsigned payload = (OBJPAYLOAD16(dst_ptr) & 255) | src_id << 8;
        FREE( OBJUNBOX(dst_ptr) );
        *((void**)dst - 1) = OBJBOX( STRDUP( OBJUNBOX(*((void**)src - 1)) ), payload);

        void *base = (void*)((void**)dst - 1);
        base = REALLOC(base, src_sz + sizeof(void*));
        *dst_ = (char*)base + sizeof(void*);
        dst = (char*)base + sizeof(void*);
        memcpy(dst, src, src_sz);

    ctor(dst);
#endif
    return dst;
}

void *obj_clone(const void *src) {
    int sz = sizeof(obj) + obj_size(src) + sizeof(array(obj*));
    enum { N = 8 }; sz = ((sz + (N - 1)) & -N);  // Round up to N-byte boundary
    obj *ptr = obj_malloc( sz );
    obj_mutate(ptr, src); // ptr->objheader = ((const obj *)src)->objheader;
    obj_loadini(ptr, obj_saveini(src));
    return ptr;
}

void* obj_merge(void *dst, const void *src) { // @testme
    char *bin = obj_savebin(src);
    return obj_mergebin(dst, bin);
}

void *obj_make(const char *str) {
    const char *T;
    const char *I = strchr(str, '['); // is_ini
    const char *J = strchr(str, '{'); // is_json
    if( !I && !J ) return 0;
    else if( I && !J ) T = I;
    else if( !I && J ) T = J;
    else T = I < J ? I : J;

    char name[64] = {0};
    if( sscanf(T+1, T == I ? "%63[^]]" : "%63[^:=]", name) != 1 ) return 0;

    int has_components = 0; // @todo: support entities too

    unsigned Tid = intern(name);
    reflect_init();
    reflect_t *found = map_find(reflects, Tid);
    if(!found) return obj_new(obj);

    obj *ptr = CALLOC(1, found->sz + (has_components+1) * sizeof(array(obj*)));
    void *ret = (T == I ? obj_mergeini : obj_mergejson)(ptr, str);
    OBJTYPES[ found->objtype ] = found->name;
    OBJ_CTOR_PTR(ptr,1,/*found->id,*/found->sz,found->objtype);
    obj_setname(ptr, name); // found->id);

    return ptr; // returns partial construction as well. @todo: just return `ret` for a more strict built/failed policy
}

#endif
