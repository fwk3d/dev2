#if !CODE

#ifndef LUAMOD_API
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#endif

#else

#pragma comment(lib, "buildvm")
#pragma comment(lib, "lua51")
#pragma comment(lib, "luajit")
#pragma comment(lib, "minilua")

#endif
