// app

#if !CODE

enum {
    APP_MSAA2 = 0x02,
    APP_MSAA4 = 0x04,
    APP_MSAA8 = 0x08,

    APP_TRANSPARENT = 0x10,
    APP_SQUARE = 0x20,
};

API void  app_create(float scale, unsigned flags);
API int   app_swap();

API const char *app_name();

// available commands:
// show 0/1, icon image.png, title str, clipboard str, vsync, ...

API int   app_send(const char *cmd, const char *val);

// available commands:
// clipboard, ...

API const char* app_recv(const char *cmd);

#else

extern int app_msaa;
extern int app_keep;
extern SDL_Window *window;
/*static*/ float clear_color[4] = { 32/255., 32/255., 32/255., 1. };
char apptitle[128];

const char *app_name() {
#   ifndef APP_NAME
#   define APP_NAME ifdef(ems, "", (__argv ? __argv[0] : ""))
#   endif
    static char buffer[256] = {0};
    if( !buffer[0] ) {
        char s[256];
        strncpy(s, APP_NAME, 256);
        char *a = strrchr(s, '/');
        char *b = strrchr(s, '\\');
        strncpy(buffer, a > b ? a+1 : b > a ? b+1 : s, 256);
        if(strendi(buffer, ".exe")) buffer[strlen(buffer) - 4] = 0;
    }
    return buffer;
}

static
void app_icon(const char *file) {
    image_t im = image(va("%s.png", app_name()), 0, 0);
    if( im.w ) {
        SDL_Surface *surface = SDL_CreateSurfaceFrom(im.w, im.h, im.n == 3 ? SDL_PIXELFORMAT_XRGB8888 : SDL_PIXELFORMAT_RGBA8888, im.pixels, im.w*im.n);
        if( surface ) {
            SDL_SetWindowIcon(window, surface);
            SDL_DestroySurface(surface);
        }
    }
    image_destroy(&im);
}

int app_send(const char *cmd, const char *val) {
    if( command2(va("app.%s",cmd), val) ) return 1;
    return 0;
}

const
char *app_recv(const char *cmd) {
    // @todo: SDL_GetClipboardData
    // @todo: SDL_GetClipboardMimeTypes
    if( !strcmp(cmd, "clipboard") ) return SDL_GetClipboardText();

    if( !strcmp(cmd, "cores") ) return va("%d", SDL_GetNumLogicalCPUCores());

    if( !strcmp(cmd, "battery") ) {
        int secs, pct, ret = SDL_GetPowerInfo(&secs, &pct);
        if( ret == SDL_POWERSTATE_NO_BATTERY ) return "0";
        if( ret == SDL_POWERSTATE_CHARGED ) return "+100";
        if( ret == SDL_POWERSTATE_CHARGING ) return va("+%d", pct);
        if( ret == SDL_POWERSTATE_ON_BATTERY) return va("-%d", pct);
        return NULL;
    }

    // app folder utils
    if(!strcmp(cmd, "APPNAME")) return app_name();
    if(!strcmp(cmd, "APPDIR")) return SDL_GetBasePath();
    if(!strcmp(cmd, "APPDATA")) { static const char *s = 0; return s ? s : (s = SDL_GetPrefPath("fwk2", __argv[0])); }
    if(!strcmp(cmd, "HOME")) return SDL_GetUserFolder(SDL_FOLDER_HOME);        /**< The folder which contains all of the current user's data, preferences, and documents. It usually contains most of the other folders. If a requested folder does not exist, the home folder can be considered a safe fallback to store a user's documents. */
    if(!strcmp(cmd, "DESKTOP")) return SDL_GetUserFolder(SDL_FOLDER_DESKTOP);     /**< The folder of files that are displayed on the desktop. Note that the existence of a desktop folder does not guarantee that the system does show icons on its desktop; certain GNU/Linux distros with a graphical environment may not have desktop icons. */
    if(!strcmp(cmd, "DOCUMENTS")) return SDL_GetUserFolder(SDL_FOLDER_DOCUMENTS);   /**< User document files, possibly application-specific. This is a good place to save a user's projects. */
    if(!strcmp(cmd, "DOWNLOADS")) return SDL_GetUserFolder(SDL_FOLDER_DOWNLOADS);   /**< Standard folder for user files downloaded from the internet. */
#if 0
    if(!strcmpi(cmd, "MUSIC")) return SDL_GetUserFolder(SDL_FOLDER_MUSIC);       /**< Music files that can be played using a standard music player (mp3, ogg...). */
    if(!strcmpi(cmd, "PICTURES")) return SDL_GetUserFolder(SDL_FOLDER_PICTURES);    /**< Image files that can be displayed using a standard viewer (png, jpg...). */
    if(!strcmpi(cmd, "PUBLICSHARE")) return SDL_GetUserFolder(SDL_FOLDER_PUBLICSHARE); /**< Files that are meant to be shared with other users on the same computer. */
    if(!strcmpi(cmd, "SAVEDGAMES")) return SDL_GetUserFolder(SDL_FOLDER_SAVEDGAMES);  /**< Save files for games. */
    if(!strcmpi(cmd, "SCREENSHOTS")) return SDL_GetUserFolder(SDL_FOLDER_SCREENSHOTS); /**< Application screenshots. */
    if(!strcmpi(cmd, "TEMPLATES")) return SDL_GetUserFolder(SDL_FOLDER_TEMPLATES);   /**< Template files to be used when the user requests the desktop environment to create a new file in a certain folder, such as "New Text File.txt".  Any file in the Templates folder can be used as a starting point for a new file. */
    if(!strcmpi(cmd, "VIDEOS")) return SDL_GetUserFolder(SDL_FOLDER_VIDEOS);      /**< Video files that can be played using a standard video player (mp4, webm...). */
#endif

    abort();
    return NULL;
}



int  sdl3_init(float scale, unsigned flags);
void sdl3_tick();
void sdl3_swap();
void sdl3_quit(void);

void app_quit(void) {
    app_send("alpha","0");
    event("quit");
    sdl3_quit();
}

void app_create(float scale, unsigned flags) {
    if( !window ) {
        if(flags & APP_TRANSPARENT) memset(clear_color, 0, 4*sizeof(clear_color[0]));

        sdl3_init(scale, flags);
        event("init");

        const char *appname = app_recv("APPNAME");
        const char *icon = va("%s.png", appname);
        if( is_file(icon) ) app_send("icon", icon);
        app_send("title", appname);
        app_send("show", "1");

        atexit(app_quit);

        sdl3_tick();
        event("tick");
    }
}

int app_swap() {
    do_once {
        // send argv commands to all registered subsystems
        for( int i = 1; i < argc(); ++i ) {
            if( argv(i)[0] == '-' && argv(i)[1] == '-' ) {
                command(argv(i)+2);
            }
        }

        PRINTF(ANSI_GREEN("boot %.2fs\n"), app_time());
        event("boot");
    }

    // fps limiter
    int must_render = limit_fps( target_fps );

    SDL_SetWindowTitle(window, va("%5.2ffps/%.2ffps %s", (float)window_fps(), target_fps, apptitle));

    // if( must_render )
        event("draw");

#if 1
    sdl3_swap();
    glClearColor(clear_color[0] * clear_color[3], clear_color[1] * clear_color[3], clear_color[2] * clear_color[3], clear_color[3]);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
#endif
    event("swap");

    sdl3_tick();

    //if( must_render )
        event("tick");

    return app_keep;
}

int execA(const char *cmd, const char *val) {
    if( !strcmp(cmd, "app.title") ) return snprintf(apptitle, sizeof(apptitle), "%s", val), 1;
    if( !strcmp(cmd, "app.show") ) return (val[0] == '1' ? SDL_ShowWindow : SDL_HideWindow)(window), 1;
    if( !strcmp(cmd, "app.icon") ) return app_icon(val), 1;

    if( !strcmp(cmd, "app.focus") ) return SDL_RaiseWindow(window), 1;
    
    if( !strcmp(cmd, "app.color") )  return clear_color[0] = clear_color[1] = clear_color[2] = atoi(val), 1; // @fixme: atoi3,RGB
    if( !strcmp(cmd, "app.chroma") ) return clear_color[3] = (val[0] == '1') * 255, 1;
    if( !strcmp(cmd, "app.alpha" ) ) return SDL_SetWindowOpacity(window, atof(val)), 1;

    // @todo: 0 = windowed, 1 = borderless, 2..N = exclusive
    if( !strcmp(cmd, "app.fullscreen") ) {
        int numdp; SDL_DisplayID *dps = SDL_GetDisplays(&numdp);
        int monitor = dps[0];

        int count;
        SDL_DisplayMode **modes = SDL_GetFullscreenDisplayModes(monitor, &count);

        puts("Available fullscreen modes:");
        puts("00) Windowed\n01) Borderless");
        for( int i = 0; i < count; ++i ) {
            SDL_DisplayMode *m = modes[i];
            printf("%02d) %.0fx%.0fx%.0f\n", i+2, m->w * m->pixel_density, m->h * m->pixel_density, m->refresh_rate);               
        }

        if( atoi(val) >= 2 ) {
            int chosen = (atoi(val)-2) < count ? (atoi(val)-2) : 0;

            if(!SDL_SetWindowFullscreenMode(window, modes[chosen]))
                printf("Warning: %s\n", SDL_GetError());

            val = "1";
        }
        if( atoi(val) < 2 ) {
            SDL_SetWindowFullscreen(window, atoi(val));
        }
        SDL_SyncWindow(window);
        return 1;
    }

    // @todo: save_on_exit();
    if( !strcmp(cmd, "app.reload") ) return fflush(0), execv(__argv[0], __argv), exit(0), 1;

    // SDL3: The vsync parameter can be 1 to synchronize present with every vertical refresh, 2 to synchronize present with every second vertical refresh, etc., SDL_WINDOW_SURFACE_VSYNC_ADAPTIVE for late swap tearing (adaptive vsync), or SDL_WINDOW_SURFACE_VSYNC_DISABLED to disable. Not every value is supported by every driver, so you should check the return value to see whether the requested setting is supported.
    //if( !strcmp(cmd, "app.vsync") ) return SDL_SetWindowSurfaceVSync(window, atoi(val)), 1;

    // [-1] adaptive (vsync when above monitor hz and off when it's below), [0] vsync off, [1] vsync on ... [N] specific Hz
    if( !strcmp(cmd, "app.fps") ) return target_fps = atof(val), 1;

    if( !strcmp(cmd, "app.clipboard") ) return SDL_SetClipboardText(val), 1;

    return 0;
}

AUTORUN {
    events("exec", execA);
}

#endif
