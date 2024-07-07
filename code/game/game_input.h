#if !CODE

enum INPUT_ENUMS {
    // -- bits: x104 keyboard, x3 mouse, x15 gamepad, x7 window
    // keyboard gaming keys (53-bit): first-class keys for gaming
    KEY_0,KEY_1,KEY_2,KEY_3,KEY_4,KEY_5,KEY_6,KEY_7,KEY_8,KEY_9,   KEY_TICK,KEY_BS,           KEY_ESC,
    KEY_TAB,   KEY_Q,KEY_W,KEY_E,KEY_R,KEY_T,KEY_Y,KEY_U,KEY_I,KEY_O,KEY_P,
    KEY_CAPS,     KEY_A,KEY_S,KEY_D,KEY_F,KEY_G,KEY_H,KEY_J,KEY_K,KEY_L, KEY_ENTER,
    KEY_LSHIFT,       KEY_Z,KEY_X,KEY_C,KEY_V,KEY_B,KEY_N,KEY_M,        KEY_RSHIFT,            KEY_UP,
    KEY_LCTRL,KEY_LALT,               KEY_SPACE,                KEY_RALT,KEY_RCTRL,  KEY_LEFT,KEY_DOWN,KEY_RIGHT,

    // for completeness, secondary keys below (52-bit). beware!
    KEY_INS,KEY_HOME,KEY_PGUP,KEY_DEL,KEY_END,KEY_PGDN, // beware: different behavior win/osx (also, osx: no home/end).
    KEY_LMETA,KEY_RMETA,KEY_MENU,KEY_PRINT,KEY_PAUSE,KEY_SCROLL,KEY_NUMLOCK, // beware: may trigger unexpected OS behavior. (@todo: add RSHIFT here for win?)
    KEY_MINUS,KEY_EQUAL,KEY_LSQUARE,KEY_RSQUARE,KEY_SEMICOLON,KEY_QUOTE,KEY_HASH,KEY_BAR,KEY_COMMA,KEY_DOT,KEY_SLASH, // beware: non-us keyboard layouts
    KEY_F1,KEY_F2,KEY_F3,KEY_F4,KEY_F5,KEY_F6,KEY_F7,KEY_F8,KEY_F9,KEY_F10,KEY_F11,KEY_F12, // beware: complicated on laptops/osx
    KEY_PAD1,KEY_PAD2,KEY_PAD3,KEY_PAD4,KEY_PAD5,KEY_PAD6,KEY_PAD7,KEY_PAD8,KEY_PAD9,KEY_PAD0, // beware: complicated on laptops
    KEY_PADADD,KEY_PADSUB,KEY_PADMUL,KEY_PADDIV,KEY_PADDOT,KEY_PADENTER, // beware: complicated on laptops

    MOUSE_L, MOUSE_M, MOUSE_R, // @todo: MOUSE_CLICKS,
    GAMEPAD_CONNECTED, GAMEPAD_A, GAMEPAD_B, GAMEPAD_X, GAMEPAD_Y,
    GAMEPAD_UP, GAMEPAD_DOWN, GAMEPAD_LEFT, GAMEPAD_RIGHT, GAMEPAD_MENU, GAMEPAD_START,
    GAMEPAD_LB, GAMEPAD_RB, GAMEPAD_LTHUMB, GAMEPAD_RTHUMB,
    WINDOW_BLUR, WINDOW_FOCUS, WINDOW_CLOSE, WINDOW_MINIMIZE, WINDOW_MAXIMIZE, WINDOW_FULLSCREEN, WINDOW_WINDOWED, // MINI/MAXI/RESTORED, SHOWN/HIDDEN

    // -- floats: x7 gamepad, x3 mouse, x4 touch, x4 window
    GAMEPAD_LPAD, GAMEPAD_LPADX = GAMEPAD_LPAD, GAMEPAD_LPADY,
    GAMEPAD_RPAD, GAMEPAD_RPADX = GAMEPAD_RPAD, GAMEPAD_RPADY,
    GAMEPAD_LTRIGGER, GAMEPAD_LT = GAMEPAD_LTRIGGER, GAMEPAD_RTRIGGER, GAMEPAD_RT = GAMEPAD_RTRIGGER, GAMEPAD_BATTERY,
    MOUSE, MOUSE_X = MOUSE, MOUSE_Y, MOUSE_W,
    TOUCH_X1, TOUCH_Y1, TOUCH_X2, TOUCH_Y2,
    WINDOW_RESIZE, WINDOW_RESIZEX = WINDOW_RESIZE, WINDOW_RESIZEY, WINDOW_ORIENTATION, WINDOW_BATTERY,

    // -- strings: x2 gamepad
    GAMEPAD_GUID, GAMEPAD_NAME,

    INPUT_MAX
};
// these aliases do check both left and right counterparts
enum INPUT_ALIASES {
    KEY_SHIFT = KEY_LSHIFT,
    KEY_ALT = KEY_LALT,
    KEY_CTRL = KEY_LCTRL,
};

API float input(int key);
API float input_diff(int key);
API float input_down(int key);
API float input_held(int key);
API float input_up(int key);
API float input_repeat(int key);

#else

float input_new[INPUT_MAX]; // current frame
float input_old[INPUT_MAX]; // previous frame
float input_repeats[INPUT_MAX];

#define k(VK) k2(VK,VK)
#define k2(VK,SDL) [KEY_##VK] = SDL_SCANCODE_##SDL
const int input_remap[512] = { // 512 becomes capacity of SDL_GetKeyboardState()
    k2(ESC,ESCAPE),k(F1),k(F2),k(F3),k(F4),k(F5),k(F6),k(F7),k(F8),k(F9),k(F10),k(F11),k(F12),                k2(PRINT,PRINTSCREEN),k(PAUSE),
    k2(TICK,GRAVE),     k(1),k(2),k(3),k(4),k(5),k(6),k(7),k(8),k(9),k(0),   k2(BS,BACKSPACE),       k2(INS,INSERT),k(HOME), k2(PGUP,PAGEUP),
    k(TAB),               k(Q),k(W),k(E),k(R),k(T),k(Y),k(U),k(I),k(O),k(P),                         k2(DEL,DELETE),k(END),k2(PGDN,PAGEDOWN),
    k2(CAPS,CAPSLOCK),      k(A),k(S),k(D),k(F),k(G),k(H),k(J),k(K),k(L),    k2(ENTER,RETURN),
    k(LSHIFT),                k(Z),k(X),k(C),k(V),k(B),k(N),k(M),                   k(RSHIFT),                       k(UP),
    k(LCTRL),k(LALT),                      k(SPACE),                         k(RALT),k(RCTRL),              k(LEFT),k(DOWN),k(RIGHT),
};
#undef k2
#undef k

void input_tick() {
    memcpy(input_old, input_new, countof(input_new) * sizeof(input_new[0]));
    memset(input_new, 0, countof(input_new) * sizeof(input_new[0]));

    static int numkeys;
    static const bool *keys;
    if(!keys) keys = SDL_GetKeyboardState(&numkeys);
    for( int i = 0; i < numkeys; ++i)
        if( input_remap[i] )
            input_new[ i ] = keys[ input_remap[i] ];

    mouse_t m = mouse(); // move this to mouse_recv();
    input_new[ MOUSE_L ] = m.l;
    input_new[ MOUSE_M ] = m.m;
    input_new[ MOUSE_R ] = m.r;
    input_new[ MOUSE_X ] = m.x;
    input_new[ MOUSE_Y ] = m.y;
    input_new[ MOUSE_W ] = m.wheel; // MouseWheelH

    for( int vk = 0; vk < INPUT_MAX; ++vk ) {
        input_repeats[vk] *= !!input_new[vk];
        input_repeats[vk] += !!input_new[vk];
    }
}

static
float input_get(float *buffer, int vk) {
    // special cases: plain shift/alt/ctrl enums will also check right counterparts
    if( vk == KEY_ALT ) return !!buffer[KEY_LALT] | !!buffer[KEY_RALT];
    if( vk == KEY_CTRL ) return !!buffer[KEY_LCTRL] | !!buffer[KEY_RCTRL];
    if( vk == KEY_SHIFT ) return !!buffer[KEY_LSHIFT] | !!buffer[KEY_RSHIFT];
    return buffer[ vk ];
}

float input(int vk) {
    return input_get(input_new, vk);
}
float input_diff(int vk) {
    return input_get(input_new, vk) - input_get(input_old, vk);
}
float input_down(int vk) {
    return input_diff(vk) > 0;
}
float input_up(int vk) {
    return input_diff(vk) < 0;
}
float input_held(int vk) {
    return (input_diff(vk) * input_diff(vk)) < 0.1f;
}
float input_repeat(int vk) {
    return input_repeats[vk] == 1 || input_repeats[vk] > 32;
}

#endif
