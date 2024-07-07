// playground tests
// - rlyeh, public domain
//
// # quickstart
// Double click MAKE.bat, or invoke `MAKE.bat`, or invoke `MAKE.bat source_file.c`

#include "engine.h"

typedef struct app_t {
    const char *name;
    int(*main)();
} app_t;

array(app_t) apps;

const char *trim(const char *pathfile) {
    // clean up. returns either extension paths or demo names
    if( strstr(pathfile, "ext-") ) return file_path(strstr(pathfile, "ext-"));
    const char *name = file_name(pathfile);
    return strchr(name, '-') ? strchr(name, '-')+1 : name;
}

int find_demo(const char *name) {
    if( isalpha(*name) )
    for( int i = 0; i < array_count(apps); ++i ) {
        if( strstri(trim(apps[i].name), name) ) return i;
    }
    return isdigit(*name) ? atoi(name) : -1;
}

int main() {
    if( argc() > 1 && is_file(argv(1)) ) return lua_init(), luaj_init(), lua_runfile(argv(1)), 0;

    int demo = argc() > 1 ? find_demo(argv(1)) : -1;
    if( demo >= 0 && demo < array_count(apps) ) return apps[demo].main();

    app_create(100, APP_TRANSPARENT|APP_MSAA2);
    command("ui.debug=0");

    static int open = UI_OPEN | UI_CENTER;
    while( app_swap() && !input(KEY_ESC) && (open & UI_OPEN) ) {
        if( ui_window("Demo Launcher", &open) ) {
            for( int i = 0; i < array_count(apps); ++i) {
            int choice = ui_buttons(2, va("%d: %s", i, trim(apps[i].name)), "...");
            if( choice == 1 ) app_send("alpha","0"), app_send("show","0"), system(va("%s %d", argv(0), i)), app_send("show","1"), app_send("alpha", "1");
            if( choice == 2 ) system(va("%s \"%s\"", ifdef(win32,"start \"\"",ifdef(osx,"open","xdg-open")), apps[i].name));
            }
            ui_window_end();
        }
    }
    return 0;
}

#pragma warning(disable : 4716)
#define ma1n(file,func) static macro(x); int func(); AUTORUN { array_push(apps, ((app_t){file, func})); } int func
#define main            ma1n( __FILE__, CONCAT(app,__COUNTER__) )

#include "art/00-null.c"
#include "art/01-window.c"
#include "art/02-imgui.c"
#include "art/03-opengl.c"
#include "art/04-image.c"
#include "art/05-ddraw.c"
#include "art/06-fx.c"
#include "art/07-font.c"
#include "art/08-input.c"
#include "art/09-shadertoy.c"
#include "art/10-tiled.c"
#include "art/11-sprite.c"
#include "art/12-sprite3d.c"
#include "art/13-model.c"
#include "art/14-script.c"
#include "art/15-audio.c"
#include "ext/ext-demos.c"
