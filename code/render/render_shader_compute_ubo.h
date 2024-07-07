// ubo

#if !CODE

API unsigned ubo_create(void *data, int size, unsigned usage);
API void       ubo_update(unsigned ubo, int offset, void *data, int size);
API void       ubo_bind(unsigned ubo, unsigned unit);
API void       ubo_unbind(unsigned unit);
API void     ubo_destroy(unsigned ubo);

#else

// ubo

unsigned ubo_create(void *data, int size, unsigned usage) {
    static GLuint gl_usage[] = { GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, GL_DYNAMIC_COPY, GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY };
    GLuint ubo;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, size, data, gl_usage[usage]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return ubo;
}

void ubo_update(unsigned ubo, int offset, void *data, int size) {
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ubo_bind(unsigned ubo, unsigned unit) {
    glBindBufferBase(GL_UNIFORM_BUFFER, unit, ubo);
}

void ubo_unbind(unsigned unit) {
    glBindBufferBase(GL_UNIFORM_BUFFER, unit, 0);
}

void ubo_destroy(unsigned ubo) {
    glDeleteBuffers(1, &ubo);
}

#endif
