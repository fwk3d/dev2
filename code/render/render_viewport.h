// -----------------------------------------------------------------------------
// viewports

API void     viewport_color(unsigned color);
API void     viewport_clear(bool color, bool depth);
API void     viewport_clip(vec2 from, vec2 to);
API void     viewport_area(vec2 from, vec2 to);

#if CODE

void viewport_color(unsigned color) {
    unsigned r = (color >>  0) & 255;
    unsigned g = (color >>  8) & 255;
    unsigned b = (color >> 16) & 255;
    unsigned a = (color >> 24) & 255;
    glClearColor(r, g, b, a);
}

void viewport_clear(bool color, bool depth) {
    glClearDepthf(gl_reversez ? 0.0f : 1.0f);
    glClearStencil(0);
    glClear((color ? GL_COLOR_BUFFER_BIT : 0) | (depth ? GL_DEPTH_BUFFER_BIT : 0));
}

void viewport_clip(vec2 from, vec2 to) {
    float x = from.x, y = from.y, w = to.x-from.x, h = to.y-from.y;

    // y = window_height()-y-h;
    glViewport(x, y, w, h);
    glScissor(x, y, w, h);
}

void viewport_area(vec2 from, vec2 to) {
    float x = from.x, y = from.y, w = to.x-from.x, h = to.y-from.y;
    glViewport(x, y, w, h);
}

#endif
