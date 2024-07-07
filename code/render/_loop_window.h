

// ----------------------------------------------------------------------------

int window_frame_begin() {
    double now = paused ? t : glfwGetTime();
    dt = now - t;
    t = now;

#if !ENABLE_RETAIL
    char *st = window_stats();
    static double timer = 0;
    timer += window_delta();
    if( timer >= 0.25 ) {
        glfwSetWindowTitle(window, st);
        timer = 0;
    }
#else
    glfwSetWindowTitle(window, title);
#endif

    return 1;
}
