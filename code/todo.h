// @todo: rename events()->hooks()
// @todo useful? before SDL_init(): SDL_SetAppMetadata("Example App", "1.0", "com.example.app");

/* 

@todo: find a way to fix SDL click passthrough on transparent windows + sdl3 backend. idea:

{
SDL_Window* window = SDL_GL_GetCurrentWindow(); // SDL_GetCurrentWindow();
SDL_Surface *surface = SDL_GetWindowSurface(window);
SDL_SetWindowShape(window, surface);
}

*/

#if !CODE

#define EVAL atof

#define ENABLE_PROFILER 1

#define app_width() ((int)igGetIO()->DisplaySize.x)
#define app_height() ((int)igGetIO()->DisplaySize.y)
#define app_time() time_ss()
#define app_path() app_recv("APPDIR")
#define app_delta() (igGetIO()->DeltaTime)
#define app_resize(w,h) SDL_SetWindowSize(window,w,h)
#define app_frame render_frame
#define app_reload() do { \
    /* @todo: save_on_exit(); */ \
    fflush(0); \
    /* chdir(app_path()); */ \
    execv(__argv[0], __argv); \
    exit(0); \
} while(0)

#define input_filter_deadzone(...) vec2(0,0)
#define input_string(x) ""
#define input_use(x)
#define input_enum(x) 0
#define input_blocked() 0
#define input_block(x) 0

#define window_delta app_delta
#define window_width app_width
#define window_height app_height
#define window_time time_ss
#define window_fps render_fps
#define window_resize app_resize
#define window_frame app_frame
#define window_reload app_reload
#define window_cursor(m) do { char x[] = "0"; x[0] += !!m; mouse_send("cursor", x); } while(0)
#define window_fullscreen(x)
#define window_has_fullscreen() 0

#endif
