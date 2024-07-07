// -----------------------------------------------------------------------------
// shaders v2

#if !CODE

typedef struct shader_t {

    unsigned program;

    // debugging
    char *vs, *fs, *gs;
    char *attribs, *fragcolor;
    char *saved;

    // statics
    array(char*) defines; // pairs of key-value,key-value...

    // dynamics
    struct {
        int location;
        char *name; // type + for debugging
        union {
            bool b;
            int i;
            unsigned u;
            float f;
            vec2 v2;
            vec3 v3;
            vec4 v4;
            mat44 m;
            struct texture { unsigned tid, tunit; };
            struct cubemap { unsigned cid, cunit; };
        };
    } uniforms[64];

    uint64_t dirty; // dirty[1]; // 64/128/192... packed flags

} shader_t;

shader_t shader2(const char *vs, const char *fs, const char *attribs, const char *fragcolor, const char *defines, const char *gs);
void     shader2_apply(shader_t *s); // apply all uniforms that are dirty, provided that shader program is in use.
void     shader2_destroy(shader_t *s);

#else

static
GLuint shader2_compile( GLenum type, array(char*) sources ) {
    if( (*array_back(sources))[0] == '\0' ) return 0; // do not compile empty shaders

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, array_count(sources), sources, NULL);
    glCompileShader(shader);

    GLint status = GL_FALSE, length;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if( status == GL_FALSE ) {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        char *buf = stack(length+1);
        glGetShaderInfoLog(shader, length, NULL, buf);

        // dump log with line numbers
        for(int n = 0, e = array_count(sources); n < e; ++n) {
            const char *source = sources[n];
            for(int line = 0, i = 0; source[i] > 0; ) {
                printf("\t%d:%03d: ", n, line+1);
                while( source[i] >= 32 || source[i] == '\t' ) fputc(source[i++], stdout);
                while( source[i] > 0 && source[i] < 32 ) line += source[i++] == '\n';
                puts("");
            }
        }

        die("!ERROR: shader2_compile():\nDevice: %s\nDriver: %s\nShader/program link: %s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION), buf);
        return 0;
    }

    return shader;
}

shader_t shader2(const char *vs, const char *fs, const char *attribs, const char *fragcolor, const char *defines, const char *gs) {
    // @todo: recover .shaders/[hash]_vs/_fs/_gs from disk at this point

    // save everything
    shader_t z = {0};
    z.gs = stringf("%s", gs ? gs : "");
    z.vs = stringf("%s", vs ? vs : "");
    z.fs = stringf("%s", fs ? fs : "");
    z.attribs = stringf("%s", attribs ? attribs : "");
    z.fragcolor = stringf("%s", fragcolor ? fragcolor : "");

    // save #defines
    array_push(z.defines, stringf("#define MAX_LIGHTS %d\n", MAX_LIGHTS));
    array_push(z.defines, stringf("#define MAX_SHADOW_LIGHTS %d\n", MAX_SHADOW_LIGHTS));
    array_push(z.defines, stringf("#define NUM_SHADOW_CASCADES %d\n", NUM_SHADOW_CASCADES));

    if( defines )
    for each_substring(defines, ",", def) {
        char *it = stringf("#define %s\n",def);
        char *eq = strchr(it, '='); if( eq ) *eq = ' '; // supports "USE_IBL=1,USE_ALPHA=0,," #defines too
        array_push(z.defines, it);
    }

    // preprocess sources
    char *gspp = *z.gs ? file_preprocess(z.gs, "shaderlib/", "shader()") : NULL;
    if( gspp ) FREE(z.gs), z.gs = gspp;

    char *vspp = *z.vs ? file_preprocess(z.vs, "shaderlib/", "shader()") : NULL;
    if( vspp ) FREE(z.vs), z.vs = vspp;

    char *fspp = *z.fs ? file_preprocess(z.fs, "shaderlib/", "shader()") : NULL;
    if( fspp ) FREE(z.fs), z.fs = fspp;

    // assemble sections
    static array(const char*) sections = 0; array_resize(sections, 0);

    const char *glsl_version = ifdef(ems, "#version 300 es\n", "#version 330\n");
    array_push(sections, glsl_version);

    static char *preamble; do_once preamble = STRDUP( file_read("shaderlib/compat.gl", 0) );
    array_push(sections, preamble);
    array_push(sections, "\n");

    for each_array(z.defines, const char*, it)
    array_push(sections, it);

    array_push(sections, z.gs);
    GLuint geom = shader2_compile(GL_GEOMETRY_SHADER, sections);
    array_pop(sections);

    array_push(sections, z.vs);
    GLuint vert = shader2_compile(GL_VERTEX_SHADER, sections);
    array_pop(sections);

    array_push(sections, z.fs);
    GLuint frag = shader2_compile(GL_FRAGMENT_SHADER, sections);
    array_pop(sections);

    // compile & link
    if( vert && frag ) {
        z.program = glCreateProgram();

        if (geom)
        glAttachShader(z.program, geom);
        glAttachShader(z.program, vert);
        glAttachShader(z.program, frag);

        attribs = z.attribs;
        for( int i = 0; attribs && attribs[0]; ++i ) {
            char attrib[128] = {0};
            sscanf(attribs, "%127[^,]", attrib);
            while( attribs[0] && attribs[0] != ',' ) { attribs++; }
            while( attribs[0] && attribs[0] == ',' ) { attribs++; break; }
            if(!attrib[0]) continue;
            glBindAttribLocation(z.program, i, attrib);
        }

        ifndef(ems, // @fixme
            if(z.fragcolor)
            glBindFragDataLocation(z.program, 0, z.fragcolor));

        glLinkProgram(z.program);

        GLint status = GL_FALSE, length;
        glGetProgramiv(z.program, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            glGetProgramiv(z.program, GL_INFO_LOG_LENGTH, &length);

            char *buf = stack(length+1);
            glGetProgramInfoLog(z.program, length, NULL, buf);

            if (geom)
            puts("--- gs:"), shader_print(z.gs);
            puts("--- vs:"), shader_print(z.vs);
            puts("--- fs:"), shader_print(z.fs);

            die("ERROR: shader():\nDevice: %s\nDriver: %s\nShader/program link: %s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION), buf);
        }

        if (geom)
        glDeleteShader(geom);
        glDeleteShader(vert);
        glDeleteShader(frag);
    }

    glFinish();

    // shader compiled fine, before returning, let's parse the source and reflect the uniforms
    array(char*) props = 0;
    do_once map_init_int( shader_reflect );
    if(z.vs) for each_substring(z.vs, "\r\n", line) {
        const char *found = strstr(line, "/""//");
        if( found > line && line[0] == '/' && line[1] == '/' ) continue;
        if( found ) array_push(props, STRDUP(line));
    }
    if(z.fs) for each_substring(z.fs, "\r\n", line) {
        const char *found = strstr(line, "/""//");
        if( found > line && line[0] == '/' && line[1] == '/' ) continue;
        if( found ) array_push(props, STRDUP(line));
    }
    if(z.gs) for each_substring(z.gs, "\r\n", line) {
        const char *found = strstr(line, "/""//");
        if( found > line && line[0] == '/' && line[1] == '/' ) continue;
        if( found ) array_push(props, STRDUP(line));
    }
    if( props ) {
        map_insert(shader_reflect, z.program, props);
        for( int i = 0; i < array_count(props); ++i ) shader_apply_param(z.program, i);
    }

    // @todo: save .shaders/[hash]_vs/_fs/_gs to disk at this point
    z.saved = NULL; // stringf(".shaders/...")

    return z;
}

void shader2_apply(shader_t *s) {
    uint64_t flags = s->dirty; s->dirty = 0;
    if( !flags ) return;

    for( int i = 0; i < 64; ++i ) { // @fixme: use popcnt64()
        if( flags & (1ULL<<i) ) continue;
        int loc = s->uniforms[i].location;
        char *name = s->uniforms[i].name, type = *name;
        switch( type ) {
            default:
                if( isdigit(type) )
                    glUniform3fv(loc, type - '0', s->uniforms[i].m);
                else
                    die("unknown uniform type [%c]%s", type, name + 1);
            break; case 'b': glUniform1i(loc, s->uniforms[i].u);
            break; case 'i': glUniform1i(loc, s->uniforms[i].i);
            break; case 'u': glUniform1ui(loc, s->uniforms[i].u);
            break; case 'x': glUniform1f(loc, s->uniforms[i].f);
            break; case 'y': glUniform2fv(loc, 1, &s->uniforms[i].v2.x);
            break; case 'z': glUniform3fv(loc, 1, &s->uniforms[i].v3.x);
            break; case 'w': glUniform4fv(loc, 1, &s->uniforms[i].v4.x);
            break; case 'm': glUniformMatrix4fv(loc, 1, GL_FALSE/*GL_TRUE*/, s->uniforms[i].m);
            break; case 't': // @todo
                // glUniform1i(shader_uniform(sampler), unit);
                // glActiveTexture(GL_TEXTURE0 + unit);
                // glBindTexture(GL_TEXTURE_2D, id);
            break; case 'c':; // @todo
                // glUniform1i(shader_uniform(sampler), unit);
                // glActiveTexture(GL_TEXTURE0 + unit);
                // glBindTexture(GL_TEXTURE_CUBE_MAP, id);
        }
    }
}

void shader2_destroy(shader_t *s) {
    shader_destroy(s->program);

    FREE(s->vs);
    FREE(s->fs);
    FREE(s->gs);;
    FREE(s->attribs);
    FREE(s->fragcolor);
    FREE(s->saved);

    for each_array(s->defines, char*, str) FREE(str);
    array_free(s->defines);

    for( int i = 0; i < countof(s->uniforms); ++i) FREE(s->uniforms[i].name);

    *s = ((shader_t){0});
}

#endif
