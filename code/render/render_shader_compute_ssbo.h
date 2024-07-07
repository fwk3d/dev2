// ssbo

#if !CODE

/// `STATIC`, `DYNAMIC` AND `STREAM` specify the frequency at which we intend to access the data.
/// `DRAW` favors CPU->GPU operations.
/// `READ` favors GPU->CPU operations.
/// `COPY` favors CPU->GPU->CPU operations.

enum SSBO_ACCESS {
    SSBO_READ = BUFFER_READ,
    SSBO_WRITE = BUFFER_WRITE,
    SSBO_READ_WRITE = BUFFER_READ_WRITE
};

/// Create Shader Storage Buffer Object
/// !!! `data` can be NULL
/// data: optional pointer to data to upload
/// len: buffer size, must not be 0
/// usage: buffer usage policy
/// see: SSBO_USAGE
API unsigned ssbo_create(const void *data, int len, unsigned usage);

/// Destroys an SSBO resource
API void ssbo_destroy(unsigned ssbo);

/// Updates an existing SSBO
/// !!! `len` can not exceed the original buffer size specified in `ssbo_create` !
/// offset: offset to buffer memory
/// len: amount of data to write
/// data: pointer to data we aim to write, can not be NULL
API void ssbo_update(int offset, int len, const void *data);

/// Bind an SSBO resource to the provided bind unit index
/// ssbo: resource object
/// unit: bind unit index
API void ssbo_bind(unsigned ssbo, unsigned unit);

/// Map an SSBO resource to the system memory
/// !!! Make sure to `ssbo_unmap` the buffer once done working with it.
/// access: buffer access policy
/// return: pointer to physical memory of the buffer
/// see: SSBO_ACCESS
API void *ssbo_map(unsigned access);

/// Unmaps an SSBO resource
/// !!! Pointer provided by `ssbo_map` becomes invalid.
API void ssbo_unmap();

/// Unbinds an SSBO resource
API void ssbo_unbind();

#else

unsigned ssbo_create(const void *data, int len, unsigned usage){
    static GLuint gl_usage[] = { GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, GL_DYNAMIC_COPY, GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY };
    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, len, data, gl_usage[usage]);
    return ssbo;
}

void ssbo_destroy(unsigned ssbo){
    glDeleteBuffers(1, &ssbo);
}

void ssbo_update(int offset, int len, const void *data){
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, len, data);
}

void *ssbo_map(unsigned access){
    static GLenum gl_access[] = {GL_READ_ONLY, GL_WRITE_ONLY, GL_READ_WRITE};
    return glMapBuffer(GL_SHADER_STORAGE_BUFFER, gl_access[access]);
}
void ssbo_unmap(){
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void ssbo_bind(unsigned ssbo, unsigned unit){
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, unit, ssbo);
}

void ssbo_unbind(){
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

#endif
