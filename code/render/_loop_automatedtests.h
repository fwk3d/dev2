int window_swap() {
    static uint64_t capture_frame = 0;
    if( cook_done && ++capture_frame == tests_captureframes() ) {
        mkdir( "tests/out", 0777 );
        const char *screenshot_file = va("tests/out/%s.png", app_name());

        int n = 3;
        void *rgb = screenshot(n);
        stbi_flip_vertically_on_write(true);
        if(!stbi_write_png(screenshot_file, w, h, n, rgb, n * w) ) {
            die("!could not write screenshot file `%s`\n", screenshot_file);
        }
        return 0;
    }

    return 1;
}
