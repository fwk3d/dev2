// -----------------------------------------------------------------------------
// config directives

#ifndef ENABLE_FASTCALL_LUA
#define ENABLE_FASTCALL_LUA     1 ///+
#endif

#ifndef ENABLE_PROFILER
#define ENABLE_PROFILER         ifdef(retail, 0, 1)  ///+
#endif

#ifndef ENABLE_SELFIES
#define ENABLE_SELFIES          0 ///+
#endif

#ifndef ENABLE_MEMORY_POISON
#define ENABLE_MEMORY_POISON    ifdef(debug, 1, 0) ///+
#endif

#ifndef ENABLE_MEMORY_LEAKS
#define ENABLE_MEMORY_LEAKS     0 ///+
#endif

#ifndef ENABLE_LINUX_CALLSTACKS
#define ENABLE_LINUX_CALLSTACKS 0 ///+
#endif

#ifndef ENABLE_AUTOTESTS
#define ENABLE_AUTOTESTS        ifdef(debug, ifndef(ems, 1, 0), 0) ///+
#endif

#ifndef ENABLE_RETAIL
#define ENABLE_RETAIL           0 // ifdef(retail, 1, 0) ///+
#endif

#ifndef ENABLE_COOK
#define ENABLE_COOK             ifdef(retail, 0, 1) ///+
#endif

#ifndef ENABLE_RPMALLOC
#define ENABLE_RPMALLOC         0 // ifdef(tcc, 0, 1) // forbidden on tcc because of lacking TLS support
#endif

#ifndef GLOBAL_FRUSTUM_ENABLED
#define GLOBAL_FRUSTUM_ENABLED 1 ///+
#endif

#ifndef GLOBAL_FRUSTUM_DEBUG_DRAW
#define GLOBAL_FRUSTUM_DEBUG_DRAW 0 ///+
#endif

#ifndef GLOBAL_FRUSTUM_FOV_MULTIPLIER
#define GLOBAL_FRUSTUM_FOV_MULTIPLIER 1.0f ///+
#endif

#ifndef GLOBAL_FX_PASS_ENABLED
#define GLOBAL_FX_PASS_ENABLED 0 ///+
#endif

#ifndef ENABLE_REVERSE_Z
#define ENABLE_REVERSE_Z 0 //ifdef(ems, 0, 1) ///+
#endif

#ifndef MAX_LIGHTS
#define MAX_LIGHTS 96
#endif

#ifndef MAX_SHADOW_LIGHTS
#define MAX_SHADOW_LIGHTS 8
#endif

#ifndef NUM_SHADOW_CASCADES
#define NUM_SHADOW_CASCADES 4
#endif

#ifndef SHADOW_CASCADE_BLEND_REGION
#define SHADOW_CASCADE_BLEND_REGION 0.2f
#endif

#ifndef DEFAULT_VSYNC
#define DEFAULT_VSYNC WINDOW_VSYNC // 0, WINDOW_VSYNC, WINDOW_VSYNC_ADAPTIVE
#endif

#ifndef DEFAULT_COOK_ON_DEMAND
#define DEFAULT_COOK_ON_DEMAND ifdef(tcc,1,0) // 0 // 1
#endif

#ifndef DEFAULT_SHADOW_VSM_RESOLUTION
#define DEFAULT_SHADOW_VSM_RESOLUTION 256
#endif

#ifndef DEFAULT_SHADOW_CSM_RESOLUTION
#define DEFAULT_SHADOW_CSM_RESOLUTION 2048
#endif
