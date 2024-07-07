#if !CODE

typedef struct gamepad_t {
    const char *name;
    bool plugged;

    bool a, b, x, y;
    bool up, down, left, right;
    bool menu, start;

    vec2 lpad, lpad_raw;
    bool lb, lthumb;

    vec2 rpad, rpad_raw;
    bool rb, rthumb;

    float lt, rt;
} gamepad_t;

API gamepad_t gamepad(unsigned gamepad_id, float deadzone); // id:[0..4], deadzone<=0.1f

API int ui_gamepad(unsigned gamepad_id);
API int ui_gamepads();

#else

int gamepad_active = 0;

gamepad_t gamepad(unsigned gamepad_id, float deadzone) {
    gamepad_t g = {"?"};
    if( gamepad_id > 4 ) return g;

    input_use(gamepad_id);
    gamepad_active = gamepad_id;

    g.name = input_string(GAMEPAD_NAME);
    g.plugged = input(GAMEPAD_CONNECTED);

    g.a = input(GAMEPAD_A);
    g.b = input(GAMEPAD_B);
    g.x = input(GAMEPAD_X);
    g.y = input(GAMEPAD_Y);
    g.up = input(GAMEPAD_UP);
    g.down = input(GAMEPAD_DOWN);
    g.left = input(GAMEPAD_LEFT);
    g.right = input(GAMEPAD_RIGHT);
    g.menu = input(GAMEPAD_MENU);
    g.start = input(GAMEPAD_START);

    g.lpad_raw.x = input(GAMEPAD_LPADX);
    g.lpad_raw.y = input(GAMEPAD_LPADY);
    g.lpad = input_filter_deadzone( g.lpad_raw, deadzone );
    g.lt = input(GAMEPAD_LT);
    g.lb = input(GAMEPAD_LB);
    g.lthumb = input(GAMEPAD_LTHUMB);

    g.rpad_raw.x = input(GAMEPAD_RPADX);
    g.rpad_raw.y = input(GAMEPAD_RPADY);
    g.rpad = input_filter_deadzone( g.rpad_raw, deadzone );
    g.rt = input(GAMEPAD_RT);
    g.rb = input(GAMEPAD_RB);
    g.rthumb = input(GAMEPAD_RTHUMB);

    return g;
}

int ui_gamepad(unsigned gamepad_id) {
    const float deadzone = 0.1f;
    gamepad_t g = gamepad(gamepad_id, deadzone);

    ui_label2("Name", g.plugged ? g.name : "(Not connected)" );

    if( !g.plugged ) ui_enable(0);

    ui_separator();

    ui_const_bool("A", g.a );
    ui_const_bool("B", g.b );
    ui_const_bool("X", g.x );
    ui_const_bool("Y", g.y );
    ui_const_bool("Up", g.up );
    ui_const_bool("Down", g.down );
    ui_const_bool("Left", g.left );
    ui_const_bool("Right", g.right );
    ui_const_bool("Menu", g.menu );
    ui_const_bool("Start", g.start );

    ui_separator();

    ui_const_float2(va("Left pad (x%.1f deadzone)", deadzone), &g.lpad.x );
    ui_const_float2("Left pad (raw)", &g.lpad_raw.x );
    ui_const_float("Left trigger", g.lt );
    ui_const_bool("Left bumper", g.lb );
    ui_const_bool("Left thumb", g.lthumb );

    ui_separator();

    ui_const_float2(va("Right pad (x%.1f deadzone)", deadzone), &g.rpad.x );
    ui_const_float2("Right pad (raw)", &g.rpad_raw.x );
    ui_const_float("Right trigger", g.rt );
    ui_const_bool("Right bumper", g.rb );
    ui_const_bool("Right thumb", g.rthumb );

    ui_enable(1);
    return 0;
}

int ui_gamepads() {
    for( int i = 0; i < 4; ++i ) ui_gamepad(i);
    return 0;
}

#endif
