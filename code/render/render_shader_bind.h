// shaders

#if !CODE

#else

static __thread unsigned last_shader = -1;

int shader_uniform(const char *name) {
    return glGetUniformLocation(last_shader, name);
}
unsigned shader_get_active() { return last_shader; }
unsigned shader_bind(unsigned program) { unsigned ret = last_shader; return glUseProgram(last_shader = program), ret; }
static inline void shader_int_(int uniform, int i)     { glUniform1i(uniform, i); }
static inline void shader_float_(int uniform, float f) { glUniform1f(uniform, f); }
static inline void shader_vec2_(int uniform, vec2 v)   { glUniform2fv(uniform, 1, &v.x); }
static inline void shader_vec3_(int uniform, vec3 v)   { glUniform3fv(uniform, 1, &v.x); }
static inline void shader_vec3v_(int uniform, int count, vec3 *v) { glUniform3fv(uniform, count, &v[0].x); }
static inline void shader_vec4_(int uniform, vec4 v)   { glUniform4fv(uniform, 1, &v.x); }
static inline void shader_mat44_(int uniform, mat44 m) { glUniformMatrix4fv(uniform, 1, GL_FALSE/*GL_TRUE*/, m); }
static inline void shader_cubemap_(int sampler, unsigned id, unsigned texture) { 
    glUniform1i(sampler, id); 
    glActiveTexture(GL_TEXTURE0 + id); 
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
}
static inline void shader_bool_(int uniform, bool x) { glUniform1i(uniform, x); }
static inline void shader_uint_(int uniform, unsigned x ) { glUniform1ui(uniform, x); }
static inline void shader_texture_unit_kind_(int kind, int sampler, unsigned id, unsigned unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(kind, id);
    glUniform1i(sampler, unit);
}
static inline void shader_texture_unit_(int sampler, unsigned id, unsigned unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
    glUniform1i(sampler, unit);
}

// public api
void shader_int(const char *uniform, int i)     { glUniform1i(shader_uniform(uniform), i); }
void shader_float(const char *uniform, float f) { glUniform1f(shader_uniform(uniform), f); }
void shader_vec2(const char *uniform, vec2 v)   { glUniform2fv(shader_uniform(uniform), 1, &v.x); }
void shader_vec3(const char *uniform, vec3 v)   { glUniform3fv(shader_uniform(uniform), 1, &v.x); }
void shader_vec3v(const char *uniform, int count, vec3 *v) { glUniform3fv(shader_uniform(uniform), count, &v[0].x); }
void shader_vec4(const char *uniform, vec4 v)   { glUniform4fv(shader_uniform(uniform), 1, &v.x); }
void shader_mat44(const char *uniform, mat44 m) { glUniformMatrix4fv(shader_uniform(uniform), 1, GL_FALSE/*GL_TRUE*/, m); }
void shader_cubemap(const char *sampler, unsigned id, unsigned unit) {
    glUniform1i(shader_uniform(sampler), unit);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}
void shader_bool(const char *uniform, bool x) { glUniform1i(shader_uniform(uniform), x); }
void shader_uint(const char *uniform, unsigned x ) { glUniform1ui(shader_uniform(uniform), x); }
void shader_texture(const char *sampler, unsigned id, unsigned unit) {
    glUniform1i(shader_uniform(sampler), unit);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}
void shader_image(texture_t t, unsigned unit, unsigned level, int layer /* -1 to disable layered access */, unsigned access){
    shader_image_unit(t.id, unit, level, layer, t.texel_type, access);
}
void shader_image_unit(unsigned texture, unsigned unit, unsigned level, int layer, unsigned texel_type, unsigned access){
    static GLenum gl_access[] = {GL_READ_ONLY, GL_WRITE_ONLY, GL_READ_WRITE};
    glBindImageTexture(unit, texture, level, layer!=-1, layer!=-1?layer:0, gl_access[access], texel_type);
}

#endif
