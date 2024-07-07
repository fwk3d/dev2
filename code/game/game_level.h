// -----------------------------------------------------------------------------
// data framework (json5, xml, compression) @todo:kvdb
// - rlyeh, public domain
//
// @todo: vec2,vec3,vec4

#if !CODE

typedef union json_t { char* s; double f; int64_t i; uintptr_t p; array(union json_t) arr; } json_t;

// json api

API bool            json_push(const char *json_content);
API const char*         json_key(const char *keypath);
API json_t*             json_find(const char *type_keypath);
API json_t              json_get(const char *type_keypath);
API int                 json_count(const char *keypath);
#define                 json_int(...)    (json_get(va("i" __VA_ARGS__)).i)
#define                 json_float(...)  (json_get(va("f" __VA_ARGS__)).f)
#define                 json_string(...) (json_get(va("s" __VA_ARGS__)).s)
#define                 json_key(...)     json_key(va(__VA_ARGS__))
#define                 json_count(...)   json_count(va(__VA_ARGS__))
API bool            json_pop();

// xml api

API int             xml_push(const char *xml_content);
API const char *        xml_string(char *key);
API unsigned            xml_count(char *key);
API array(char)         xml_base64(char *key);
#define                 xml_string(...) xml_string(va(__VA_ARGS__))       // syntax sugar: string
#define                 xml_int(...)    atoi(xml_string(__VA_ARGS__))     // syntax sugar: int
#define                 xml_float(...)  atof(xml_string(__VA_ARGS__))     // syntax sugar: float
#define                 xml_base64(...)   xml_base64(va(__VA_ARGS__))     // syntax sugar: base64 blob
#define                 xml_count(...)  xml_count(va(__VA_ARGS__))        // syntax sugar: count nodes
API void            xml_pop();

// ini api

// @todo: evaluate alt api #1
// char *ini(filename, section.key, default);
// float inif(filename, section.key, default);
// @todo: evaluate alt api #2
// char *val = ini(filename, section_key);
// int count = ini_count(filename);
// char *key = ini_key_id(filename, id);
// char *val = ini_val_id(filename, id);

typedef map(char*,char*) ini_t;

API ini_t        ini(const char *filename);
API ini_t        ini_from_mem(const char *data);
API void         ini_destroy(ini_t);

API bool         ini_write(const char *filename, const char *section, const char *key, const char *value);

API bool data_tests();

#else

static array(json5) roots;
static array(char*) sources;

bool json_push(const char *source) {
    char *source_rw = STRDUP(source);
    json5 root = {0};
    char *error = json5_parse(&root, source_rw, 0);
    if( error ) {
        FREE(source_rw);
        return false;
    } else {
        array_push(sources, source_rw);
        array_push(roots, root);
        return true;
    }
}

bool json_pop() {
    if( array_count(roots) > 0 ) {
        FREE(*array_back(sources));
        array_pop(sources);

        json5_free(array_back(roots));
        array_pop(roots);
        return true;
    }
    return false;
}

json5* json_node(const char *keypath) {
    json5 *j = array_back(roots), *r = j;
    for each_substring( keypath, "/[.]", key ) {
        r = 0;
        /**/ if( j->type == JSON5_ARRAY ) r = j = &j->array[atoi(key)];
        else if( j->type == JSON5_OBJECT && isdigit(key[0]) )
        for( int i = 0, seq = atoi(key); !r && i < j->count; ++i ) {
            if( i == seq ) {
                r = j = &j->nodes[i];
                break;
            }
        }
        else if( j->type == JSON5_OBJECT )
        for( int i = 0; !r && i < j->count; ++i ) {
            if( j->nodes[i].name && !strcmp(j->nodes[i].name, key) ) {
                r = j = &j->nodes[i];
                break;
            }
        }
        if( !j ) break;
    }
    return r;
}

int (json_count)(const char *keypath) {
    json5* j = json_node(keypath);
    return j ? j->count : 0;
}

json_t *json_find(const char *type_keypath) {
    char type = type_keypath[0];
    const char *key = type_keypath+1;
    json5 *j = json_node(key);
    if( !j ) return NULL;

    static __thread int slot = 0;
    static __thread json_t buf[128] = {0};
    slot = (slot+1) % 128;

    json_t *v = &buf[slot];
    v->i = j ? j->integer : 0;
    if(type == 's' && (!v->p || j->type == JSON5_NULL)) v->s = ""; // if_null_string
    if(type == 'f' && j && j->type == JSON5_INTEGER) v->f = j->integer;
    return v;
}

json_t json_get(const char *type_keypath) {
    char type = type_keypath[0];
    const char *key = type_keypath+1;
    json5 *j = json_node(key);

    json_t v = {0};
    v.i = j ? j->integer : 0;
    if(type == 's' && (!v.p || j->type == JSON5_NULL)) v.s = ""; // if_null_string
    if(type == 'f' && j && j->type == JSON5_INTEGER) v.f = j->integer;
    return v;
}

const char *(json_key)(const char *keypath) {
    json5 *j = json_node(keypath);
    if( !j ) return "";
    return j->name;
}

// xml impl

static __thread array(char *) xml_sources;
static __thread array(struct xml *) xml_docs;

int xml_push(const char *xml_source) {
    if( xml_source ) {
        char *src = STRDUP(xml_source), *error = 0;
        for( struct xml *doc = xml_parse(src, 0, &error); doc && !error; ) {
            array_push(xml_docs, doc);
            array_push(xml_sources, src);
            return 1;
        }
        if( error ) PRINTF("%s\n", error);
        FREE(src);
    }
    return 0;
}

void xml_pop() {
    if( array_count(xml_docs) ) {
        xml_free( *array_back(xml_docs) );
        array_pop(xml_docs);
        FREE( *array_back(xml_sources) );
        array_pop(xml_sources);
    }
}

static void *xml_path(struct xml *node, char *path, int down) {
    if( !path || !path[0] ) return node;
    if( node ) {

        char type = path[0];
        if( type == '/' ) {
            int sep = strcspn(++path, "/[@$");
            if( !sep ) type = path[0];
            else
            if( 1 ) { // path[ sep ] ) {
                char tag[32]; snprintf(tag, 32, "%.*s", sep, path);
                // Find the first sibling with the given tag name (may be the same node)
                struct xml *next = down ? xml_find_down(node, tag) : xml_find(node, tag);
                return xml_path(next, &path[ sep ], 1);
            }
        }
        if( type == '$' ) {
            return (void*)( node->down ? xml_text( node->down ) : xml_tag( node ) );
        }
        if( type == '@' ) {
            return (void*)xml_att(node, ++path);
        }
        if( type == '[' ) {
            for( int i = 0, end = atoi(++path); i < end; ++i ) { node = xml_find_next(node, xml_tag(node)); if(!node) return NULL; }
            while( isdigit(path[0]) ) ++path;
            return xml_path(node, ++path, 1);
        }
    }
    return NULL;
}

const char *(xml_string)(char *key) {
    struct xml *node = xml_path(*array_back(xml_docs), key, 0);
    if( node && strchr(key, '@') ) return (const char *)node;
    if( node && strchr(key, '$') ) return (const char *)node;
    return "";
}
unsigned (xml_count)(char *key) {
    struct xml *node = xml_path(*array_back(xml_docs), key, 0);
    if( !node ) return 0;
    const char *tag = xml_tag(node);
    unsigned count = 1;
    while( (node = xml_find_next(node, tag)) != 0) ++count;
    return count;
}
array(char) (xml_base64)(char *key) { // base64 blob
    struct xml *node = xml_path(*array_back(xml_docs), key, 0);
    if( !node ) return 0;
    if( !strchr(key, '$') ) return 0;
    const char *data = (const char*)node;
    array(char) out = base64_decode(data, strlen(data)); // either array of chars (ok) or null (error)
    return out;
}

// ----------------------------------------------------------------------------
// ini


/* ini+, extended ini format
// - rlyeh, public domain
//
// # spec
//
//   ; line comment
//   [user]             ; map section name (optional)
//   name=john          ; key and value (mapped here as user.name=john)
//   +surname=doe jr.   ; sub-key and value (mapped here as user.name.surname=doe jr.)
//   age=30             ; numbers
//   color=240          ; key and value \
//   color=253          ; key and value |> array: color[0], color[1] and color[2]
//   color=255          ; key and value /
//   color=             ; remove key/value(s)
//   color=white        ; recreate key; color[1] and color[2] no longer exist
//   []                 ; unmap section
//   -note=keys may start with symbols (except plus and semicolon)
//   -note=linefeeds are either \r, \n or \r\n.
//   -note=utf8 everywhere.
*/

static
char *ini_parse( const char *s ) {
    char *map = 0;
    int mapcap = 0, maplen = 0;
    enum { DEL, REM, TAG, KEY, SUB, VAL } fsm = DEL;
    const char *cut[6] = {0}, *end[6] = {0};
    while( *s ) {
        while( *s && (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n') ) ++s;
        /**/ if( *s == ';' ) cut[fsm = REM] = ++s;
        else if( *s == '[' ) cut[fsm = TAG] = ++s;
        else if( *s == '+' ) cut[fsm = SUB] = ++s;
        else if( *s == '=' ) cut[fsm = VAL] = ++s;
        else if( *s > ' ' && *s <= 'z' && *s != ']' ) cut[fsm = KEY] = cut[SUB] = end[SUB] = s;
        else { if( *s ) ++s; continue; }
        /**/ if( fsm == REM ) { while(*s && *s != '\r'&& *s != '\n') ++s; }
        else if( fsm == TAG ) { while(*s && *s != '\r'&& *s != '\n'&& *s != ']') ++s; end[TAG] = s; }
        else if( fsm == KEY ) { while(*s && *s >  ' ' && *s <= 'z' && *s != '=') ++s; end[KEY] = s; }
        else if( fsm == SUB ) { while(*s && *s >  ' ' && *s <= 'z' && *s != '=') ++s; end[SUB] = s; }
        else if( fsm == VAL ) { while(*s && *s >= ' ' && *s <= 127 && *s != ';') ++s; end[VAL] = s;
            while( end[VAL][-1] <= ' ' ) { --end[VAL]; }
            char buf[256] = {0}, *key = buf;
            if( end[TAG] - cut[TAG] ) key += sprintf(key, "%.*s.", (int)(end[TAG] - cut[TAG]), cut[TAG] );
            if( end[KEY] - cut[KEY] ) key += sprintf(key,  "%.*s", (int)(end[KEY] - cut[KEY]), cut[KEY] );
            if( end[SUB] - cut[SUB] ) key += sprintf(key, ".%.*s", (int)(end[SUB] - cut[SUB]), cut[SUB] );
            int reqlen = (key - buf) + 1 + (end[VAL] - cut[VAL]) + 1 + 1;
            if( (reqlen + maplen) >= mapcap ) map = REALLOC( map, mapcap += reqlen + 512 );
            sprintf( map + maplen, "%.*s%c%.*s%c%c", (int)(key - buf), buf, 0, (int)(end[VAL] - cut[VAL]), cut[VAL], 0, 0 );
            maplen += reqlen - 1;
        }
    }
    return map;
}

// @todo: evaluate alt api
// int count = ini_count(filename);
// char *key = ini_key(filename, id);
// char *val = ini_val(filename, id);

void ini_destroy(ini_t x) {
    for each_map(x, char*, k, char*, v) {
        FREE(k);
        FREE(v);
    }
    map_free(x);
}

ini_t ini_from_mem(const char *data) {
    if( !data || !data[0] ) return 0;

    char *kv = ini_parse(data);
    if( !kv ) return 0;

        ini_t map = 0;
        map_init(map, less_str, hash_str);
        for( char *iter = kv; iter[0]; ) {
            char *key = iter; while( *iter++ );
            char *val = iter; while( *iter++ );

            char **found = map_find(map, key);
            if( !found ) map_insert(map, STRDUP(key), STRDUP(val));
            assert( map_find(map,key) );
        }

    FREE( kv );

    return map;
}

ini_t ini(const char *filename) {
    char *kv = file_read(filename, 0);
    return ini_from_mem(kv);
}

bool ini_write(const char *filename, const char *section, const char *key, const char *value) {
    // this is a little hacky {
    char *data = file_read(filename, 0);
    if( data && data[0] ) {
        char *begin = strrchr(data, '[');
        char *end = strrchr(data, ']');
        if( begin && end && begin < end ) {
            char *last_section = va("%.*s", (int)(end - begin - 1), begin + 1);
            if( !strcmpi(last_section, section) ) section = 0;
        }
    }
    // }

    char *s = va("%s%s=%s\r\n", section ? va("[%s]\r\n", section) : "", key, value);
    return file_append(filename, s, strlen(s));
}

// ----------------------------------------------------------------------------

bool data_tests() {
    // data tests (json5)
    const char json5[] =
    "  /* json5 */ // comment\n"
    "  abc: 42.67, def: true, integer:0x100 \n"
    "  huge: 2.2239333e5, \n"
    "  hello: 'world /*comment in string*/ //again', \n"
    "  children : { a: 1, b: 2, c: 3 },\n"
    "  array: [+1,2,-3,4,5],    \n"
    "  invalids : [ nan, NaN, -nan, -NaN, inf, Infinity, -inf, -Infinity ],";
    if( json_push(json5) ) {
        assert( json_float("/abc") == 42.67 );
        assert( json_int("/def") == 1 );
        assert( json_int("/integer") == 0x100 );
        assert( json_float("/huge") > 2.22e5 );
        assert( strlen(json_string("/hello")) == 35 );
        assert( json_int("/children/a") == 1 );
        assert( json_int("/children.b") == 2 );
        assert( json_int("/children[c]") == 3 );
        assert( json_int("/array[%d]", 2) == -3 );
        assert( json_count("/invalids") == 8 );
        assert( isnan(json_float("/invalids[0]")) );
        assert( !json_find("/non_existing") );
        assert( PRINTF("json5 tests OK\n") );
        json_pop();
    }

    // data tests (xml)
    const char *xml = // vfs_read("test1.xml");
    "<!-- XML representation of a person record -->"
    "<person created=\"2006-11-11T19:23\" modified=\"2006-12-31T23:59\">"
    "    <firstName>Robert</firstName>"
    "    <lastName>Smith</lastName>"
    "    <address type=\"home\">"
    "        <street>12345 Sixth Ave</street>"
    "        <city>Anytown</city>"
    "        <state>CA</state>"
    "        <postalCode>98765-4321</postalCode>"
    "    </address>"
    "</person>";
    if( xml_push(xml) ) {
        assert( !strcmp("Robert", xml_string("/person/firstName/$")) );
        assert( !strcmp("Smith", xml_string("/person/lastName/$")) );
        assert( !strcmp("home", xml_string("/person/address/@type")) );
        assert( PRINTF("xml tests OK\n") );
        xml_pop();
    }

    return true;
}

#endif
