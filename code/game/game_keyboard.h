#if !CODE

API int ui_keyboard();

#else

int ui_keyboard() {
    char *keys[] = {
        "F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12",
        "ESC",
        "TICK","1","2","3","4","5","6","7","8","9","0","BS",
        "TAB","Q","W","E","R","T","Y","U","I","O","P",
        "CAPS","A","S","D","F","G","H","J","K","L","ENTER",
        "LSHIFT","Z","X","C","V","B","N","M","RSHIFT","^",
        "LCTRL","LALT","SPACE","RALT","RCTRL","<","V",">",
    };

    float rows[] = {
        12,
        1,
        12,
        11,
        11,
        10,
        8
    };

    ui_enable(0);

    for( int row = 0, k = 0; row < COUNTOF(rows); ++row ) {
        static char *buf = 0; if(buf) *buf = 0;
        for( int col = 0; col < rows[row]; ++col, ++k ) {
            assert( input_enum(keys[k]) == input_enum(va("KEY_%s", keys[k])) );
            strcatf(&buf, input(input_enum(keys[k])) ? "[%s]" : " %s ", keys[k]);
        }
        ui_label(buf);
    }

    ui_enable(1);

    return 0;
}

#endif
