#if CODE
#define LUA_IMPL
#define LUA_USE_POPEN 1 // for lite editor
#define LUADEBUGGER_C
#define error error2 // stb_vorbis
#endif

#ifndef LUAMOD_API
//#include "3rd_lua.h"
//#include "3rd_luadebugger.h"
#endif

