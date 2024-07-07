// mouse ----------------------------------------------------------------------
// - rlyeh, public domain

#if !CODE

typedef enum CURSORS {
    CURSOR_HIDE,
    CURSOR_AUTO,
    CURSOR_ARROW,
    CURSOR_HAND,
    CURSOR_TEXT,
    CURSOR_CROSS,
    CURSOR_MAX
} CURSORS;

typedef struct mouse_t {
    int x, y, l, m, r, any, wheel, cursor;
    // hovering, dragging, uiactive, vec2 clip
} mouse_t;

API mouse_t mouse();
API int     mouse_send(const char *cmd, const char *val);

API int     ui_mouse();

#else

float mouse_wheel;

mouse_t mouse() { // x,y,wheelx,wheely
    float mx, my;
    int buttons = SDL_GetMouseState(&mx, &my);

    mouse_t m = {0};
    m.x = mx;
    m.y = my;
    m.l = !!(buttons & SDL_BUTTON_LMASK);
    m.m = !!(buttons & SDL_BUTTON_MMASK);
    m.r = !!(buttons & SDL_BUTTON_RMASK);
    m.any = !!buttons;
    m.wheel = mouse_wheel;
    m.cursor = igGetMouseCursor();
    /**/ if( m.cursor == ImGuiMouseCursor_None ) m.cursor = 0;
    else if( m.cursor == ImGuiMouseCursor_Arrow ) m.cursor = 1;
    else if( m.cursor == ImGuiMouseCursor_Hand ) m.cursor = 2;
    else if( m.cursor == ImGuiMouseCursor_TextInput ) m.cursor = 3;
    else if( m.cursor == ImGuiMouseCursor_ResizeAll ) m.cursor = 4;

    // @todo: buttons 4,5 SDL_BUTTON_X1MASK SDL_BUTTON_X2MASK
    // @todo: adjust mouse coords when shader for CRT distortion is applied

    return m;
}

int g_mousecursor = 1;

void mouse_cursor(int mode) { // 0(hide),1(auto),2(arrow),3(hand),4(ibeam),5(cross)
    g_mousecursor = mode;
}
void mouse_clip(bool enabled) {
    // @todo: via cimgui
}

int mouse_send(const char *cmd, const char *val) {
    if( !strcmp(cmd, "cursor") ) return mouse_cursor(atoi(val)), 1;
    return 0;
}

void mouse_tick() {
    mouse_wheel += igGetIO()->MouseWheel; // MouseWheelH
    // imgui_sdl3 backend does reset mouse cursor every frame
    int is_auto = g_mousecursor == 1;
    if( is_auto ) return;
    // else manual cursor
    int cursor = 0;
    /**/ if( g_mousecursor == 0 ) cursor = ImGuiMouseCursor_None;
    else if( g_mousecursor == 1 ) cursor = ImGuiMouseCursor_Arrow;
    else if( g_mousecursor == 2 ) cursor = ImGuiMouseCursor_Arrow;
    else if( g_mousecursor == 3 ) cursor = ImGuiMouseCursor_Hand;
    else if( g_mousecursor == 4 ) cursor = ImGuiMouseCursor_TextInput;
    else if( g_mousecursor == 5 ) cursor = ImGuiMouseCursor_ResizeAll;
//  else if( g_mousecursor == 0 ) cursor =  ImGuiMouseCursor_ResizeNS;
//  else if( g_mousecursor == 0 ) cursor =  ImGuiMouseCursor_ResizeEW;
//  else if( g_mousecursor == 0 ) cursor =  ImGuiMouseCursor_ResizeNESW;
//  else if( g_mousecursor == 0 ) cursor =  ImGuiMouseCursor_ResizeNWSE;
    igSetMouseCursor( cursor );
}

int ui_mouse() {
    ui_enable(0);
    ui_const_float("X", input(MOUSE_X));
    ui_const_float("Y", input(MOUSE_Y));
    ui_const_float("Wheel", input(MOUSE_W));
    ui_separator();
    ui_const_bool("Left", input(MOUSE_L));
    ui_const_bool("Middle", input(MOUSE_M));
    ui_const_bool("Right", input(MOUSE_R));
    ui_separator();
    ui_enable(1);

    for( int i = 0; i < CURSOR_MAX; ++i )
        if(ui_button(va("Cursor shape #%d", i)))
            mouse_send("cursor", va("%d",i));

    return 0;
}

#endif
