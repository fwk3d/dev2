//-----------------------------------------------------------------------------
// About imgui_impl_opengl3_loader.h:
//
// We embed our own OpenGL loader to not require user to provide their own or to have to use ours,
// which proved to be endless problems for users.
// Our loader is custom-generated, based on gl3w but automatically filtered to only include
// enums/functions that we use in our imgui_impl_opengl3.cpp source file in order to be small.
//
// YOU SHOULD NOT NEED TO INCLUDE/USE THIS DIRECTLY. THIS IS USED BY imgui_impl_opengl3.cpp ONLY.
// THE REST OF YOUR APP SHOULD USE A DIFFERENT GL LOADER: ANY GL LOADER OF YOUR CHOICE.
//
// IF YOU GET BUILD ERRORS IN THIS FILE (commonly macro redefinitions or function redefinitions):
// IT LIKELY MEANS THAT YOU ARE BUILDING 'imgui_impl_opengl3.cpp' OR INCLUDING 'imgui_impl_opengl3_loader.h'
// IN THE SAME COMPILATION UNIT AS ONE OF YOUR FILE WHICH IS USING A THIRD-PARTY OPENGL LOADER.
// (e.g. COULD HAPPEN IF YOU ARE DOING A UNITY/JUMBO BUILD, OR INCLUDING .CPP FILES FROM OTHERS)
// YOU SHOULD NOT BUILD BOTH IN THE SAME COMPILATION UNIT.
// BUT IF YOU REALLY WANT TO, you can '#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM' and imgui_impl_opengl3.cpp
// WILL NOT BE USING OUR LOADER, AND INSTEAD EXPECT ANOTHER/YOUR LOADER TO BE AVAILABLE IN THE COMPILATION UNIT.
//
// Regenerate with:
//   python3 gl3w_gen.py --output ../imgui/backends/imgui_impl_opengl3_loader.h --ref ../imgui/backends/imgui_impl_opengl3.cpp ./extra_symbols.txt
//
// More info:
//   https://github.com/dearimgui/gl3w_stripped
//   https://github.com/ocornut/imgui/issues/4445
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IMGL3W_H
#define IMGL3W_H

#define GLAD_API_CALL extern
#include "../../3rd_glad.h"

#define IMGL3W_API GLAD_API_CALL

#define IMGL3W_OK 0
#define IMGL3W_ERROR_INIT -1
#define IMGL3W_ERROR_LIBRARY_OPEN -2
#define IMGL3W_ERROR_OPENGL_VERSION -3

/* gl3w api */
IMGL3W_API int imgl3wInit(void);
IMGL3W_API int imgl3wIsSupported(int major, int minor);

#endif // IMGL3W_H

#ifdef IMGL3W_IMPL

static struct { int major, minor; } version;

static int parse_version(void)
{
    if (!glGetIntegerv)
        return IMGL3W_ERROR_INIT;
    glGetIntegerv(GL_MAJOR_VERSION, &version.major);
    glGetIntegerv(GL_MINOR_VERSION, &version.minor);
    if (version.major == 0 && version.minor == 0)
    {
        // Query GL_VERSION in desktop GL 2.x, the string will start with "<major>.<minor>"
        if (const char* gl_version = (const char*)glGetString(GL_VERSION))
            sscanf(gl_version, "%d.%d", &version.major, &version.minor);
    }
    if (version.major < 2)
        return IMGL3W_ERROR_OPENGL_VERSION;
    return IMGL3W_OK;
}

int imgl3wInit(void)
{
    int res = !gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
    if (res)
        return res;
    // atexit(close_libgl);
    return parse_version();
}

int imgl3wIsSupported(int major, int minor)
{
    if (major < 2)
        return 0;
    if (version.major == major)
        return version.minor >= minor;
    return version.major >= major;
}

#endif // IMGL3W_IMPL

#ifdef __cplusplus
}
#endif
