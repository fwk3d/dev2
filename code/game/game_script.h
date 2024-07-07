#if 0

// -----------------------------------------------------------------------------
// script framework

#if !CODE

enum {
    SCRIPT_LUA = 1,
    SCRIPT_DEBUGGER = 2,

    SCRIPT_C = 4,
};

API void  script_init(); // @deprecate
API void *script_init_env(unsigned flags);
API bool  script_push(void *env);

API void    script_run(const char *script);
API void    script_runfile(const char *pathfile);

API void    script_bind_class(const char *objname, int num_methods, const char **c_names, void **c_functions);
API void    script_bind_function(const char *c_name, void *c_function);
API void    script_call(const char *lua_function);

API bool    script_tests();

API bool  script_pop();

#else

// script ---------------------------------------------------------------------

typedef lua_State lua;

// the Lua interpreter(s)
static array(lua*) Ls;

// the **current** Lua interpreter
static lua *L;

#if is(linux)
void luaopen_libengine(lua_State *L) {}
#endif

static void* script__realloc(void *userdef, void *ptr, size_t osize, size_t nsize) {
    (void)userdef;
    return ptr = REALLOC( ptr, /* (osize+1) * */ nsize );
}
static int script__traceback(lua_State *L) {
    if (!lua_isstring(L, 1)) { // try metamethod if non-string error object
        if (lua_isnoneornil(L, 1) ||
            !luaL_callmeta(L, 1, "__tostring") ||
            !lua_isstring(L, -1))
            return 1;  // return non-string error object
        lua_remove(L, 1);  // replace object with result of __tostring metamethod
    }
    luaL_traceback(L, L, lua_tostring(L, 1), 1);
    return 1;
}
static void script__error(lua_State *L, int status) {
    if (status != 0) {
        const char *errormsg = lua_tostring(L, -1);
        PRINTF( "!-- %s\n", errormsg);
        lua_pop(L, 1); // remove error message
    }
}
static int script__call(lua_State *L, int narg, int clear) {
#if ENABLE_FASTCALL_LUA
    lua_call(L, 0, 0);
    return 0;
#else
    int base = lua_gettop(L) - narg;  // function index
    lua_pushcfunction(L, script__traceback);  // push traceback function
    lua_insert(L, base);  // put it under chunk and args
    int status = lua_pcall(L, narg, (clear ? 0 : LUA_MULTRET), base);
    script__error(L, status);
    lua_remove(L, base);  // remove traceback function
    if (status != 0) lua_gc(L, LUA_GCCOLLECT, 0); // force gc in case of errors
    return status;
#endif
}

void script_bind_function(const char *c_name, void *c_function) {
    lua_pushcfunction( L, c_function );
    lua_setglobal( L, c_name );
}

void script_call(const char *lua_function) {
    lua_getglobal( L, lua_function );
    script__call( L, 0, 1 );
}

void script_bind_class(const char *classname, int num, const char **methods, void **functions) {
    lua_newtable( L );

    for( int i = 0; i < num; ++i) {
        lua_pushcfunction( L, functions[i] );
        lua_setfield( L, 1, methods[i] );
    }

    lua_setglobal( L, classname );
}

void script_run(const char *script) {
    int ret = luaL_dostring(L, script);
    if( ret != LUA_OK ) {
      PRINTF("!Script failed to run: %s\n", lua_tostring(L, -1));
      lua_pop(L, 1); // pop error message
    }
}

void script_runfile(const char *pathfile) {
    PRINTF( "Loading script '%s'\n", pathfile );
    int loadResult = luaL_loadfile( L, pathfile );

    /**/ if( loadResult == LUA_OK ) {
        script__call( L, 0, 1 );
    }
    else if( loadResult == LUA_ERRSYNTAX ) {
        PRINTF("!Script failed to load (LUA_ERRSYNTAX, '%s'): %s\n", lua_tostring( L, 1 ), pathfile );
//      lua_pop(L, 1); // pop error message
    }
    else if( loadResult == LUA_ERRMEM ) {
        PRINTF("!Script failed to load (LUA_ERRMEM): %s\n", pathfile);
    }
    else {
        PRINTF("!Script failed to load: %s\n", pathfile );
    }
}

void script_quit(void) {
    if( L ) {
        lua_close(L);
        L = 0;
    }
}
void script_init() {
    if( !L ) {
        // framework_init();

        // initialize Lua
        L = lua_newstate(script__realloc, 0); // L = luaL_newstate();

#if 0
#if !is(ems)
        // enable ffi (via luaffi)
        luaopen_ffi(L);
#endif
#endif

#if 0
        // @fixme: workaround that prevents script binding on lua 5.4.3 on top of luajit 2.1.0-beta3 on linux. lua_setglobal() crashing when accessing null L->l_G
        if(L->l_G) {
        XMACRO(BIND_ALL);
        }
#endif

        atexit(script_quit);
    }
}

bool script_tests() {
    // script test (lua)
    script_run( "-- Bye.lua\nio.write(\"script test: Bye world!, from \", _VERSION, \"\\n\")" );
    return true;
}

#undef XMACRO

// script v2 ------------------------------------------------------------------



void *script_init_env(unsigned flags) {
    if( flags & SCRIPT_LUA ) {
        lua_State *L = luaL_newstate();
        luaL_openlibs(L);

#if 0
        if( flags & SCRIPT_DEBUGGER ) {
            // Register debuggers/inspectors
            // luaL_dostringsafe(L, "I = require('inspect').inspect\n");
            dbg_setup_default(L);
        }
#endif

        lua_add_ziploader(L);
        return L;
    }

    return 0;
}

bool script_push(void *env) {
    array_push(Ls, L = env);
    return true;
}

bool script_pop() {
    L = array_count(Ls) && (array_pop(Ls), array_count(Ls)) ? *array_back(Ls) : NULL;
    return !!array_count(Ls);
}

#endif

#endif
