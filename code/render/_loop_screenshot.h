
void window_frame_end() {
    // flush batching systems that need to be rendered before frame swapping. order matters.
    {
        font_goto(0,0);
        void touch_flush();
        touch_flush();
        sprite_flush();

        ui_render();
    }

#if !is(ems)
    // save screenshot if queued
    if( screenshot_file[0] ) {
        int n = 3;
        void *rgb = screenshot(n);
        stbi_flip_vertically_on_write(true);
        if(!stbi_write_png(screenshot_file, w, h, n, rgb, n * w) ) {
            die("!could not write screenshot file `%s`\n", screenshot_file);
        }
        screenshot_file[0] = 0;
    }
    if( record_active() ) {
        void record_frame();
        record_frame();
    }
#endif
}
