// -----------------------------------------------------------------------------
// shaders

API unsigned shader(const char *vs, const char *fs, const char *attribs, const char *fragcolor, const char *defines);
API unsigned shader_geom(const char *gs, const char *vs, const char *fs, const char *attribs, const char *fragcolor, const char *defines);
API unsigned shader_bind(unsigned program);
API      int shader_uniform(const char *name);
API     void shader_bool(const char *uniform, bool i );
API     void shader_int(const char *uniform, int i);
API     void shader_uint(const char *uniform, unsigned i );
API     void shader_float(const char *uniform, float f);
API     void shader_vec2(const char *uniform, vec2 v);
API     void shader_vec3(const char *uniform, vec3 v);
API     void shader_vec3v(const char *uniform, int count, vec3 *v);
API     void shader_vec4(const char *uniform, vec4 v);
API     void shader_mat44(const char *uniform, mat44 m);
API     void shader_texture(const char *sampler, unsigned texture, unsigned unit);
API     void shader_cubemap(const char *sampler, unsigned texture, unsigned unit);
API unsigned shader_get_active();
API void     shader_destroy(unsigned shader);

// reflection. [0..N] are shader properties

API unsigned     shader_properties(unsigned shader);
API char**       shader_property(unsigned shader, unsigned property_no);

API void         shader_apply_param(unsigned shader, unsigned param_no);
API void         shader_apply_params(unsigned shader, const char *parameter_mask);

API int          ui_shader(unsigned shader);
API int          ui_shaders();


#if CODE

void shader_print(const char *source) {
    for(int line = 0, i = 0; source[i] > 0; ) {
        printf("\t%03d: ", line+1);
        while( source[i] >= 32 || source[i] == '\t' ) fputc(source[i++], stdout);
        while( source[i] > 0 && source[i] < 32 ) line += source[i++] == '\n';
        puts("");
    }
}

// sorted by shader handle. an array of properties per shader. properties are plain strings.
static __thread map(unsigned, array(char*)) shader_reflect;

static
GLuint shader_compile( GLenum type, const char *source ) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, (const char **)&source, NULL);
    glCompileShader(shader);

    GLint status = GL_FALSE, length;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if( status == GL_FALSE ) {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
//      ASSERT(length < 2048); char buf[2048] = { 0 };
        char *buf = stack(length+1);
        glGetShaderInfoLog(shader, length, NULL, buf);

        // dump log with line numbers
        shader_print( source );
        die("!ERROR: shader_compile():\nDevice: %s\nDriver: %s\nShader/program link: %s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION), buf);
        return 0;
    }

    return shader;
}

unsigned shader(const char *vs, const char *fs, const char *attribs, const char *fragcolor, const char *defines){
    return shader_geom(NULL, vs, fs, attribs, fragcolor, defines);
}

static
char *file_preprocess(const char *src, const char *path, const char *parent_function) { // must FREE() after use
    if (!src) return NULL;

    char *includes = NULL;
    for each_substring(src, "\n", line) {
        if (strbeg(line,"#include")) { // } || strmatch(line, "*#*include")) {
            const char *start = strstri(line, "\"");
            const char *end = strstri(start+1, "\"");
            if (start && end) {
                char *filename = va("%s%.*s", path, (int)(end-start-1), start+1);
                char *included = file_read(filename, 0);
                if ( !included ) { // fallback: try without path. file_read() will scan it for us
                    filename += strlen(path);
                    included = file_read(filename, 0);
                }
                if (included) {
                    char *nested_includes = file_preprocess(included, path, parent_function);
                    includes = strcatf(&includes, "// begin: %s\n%s\n// end: %s\n", line, nested_includes ? nested_includes : "", line); //@leak
                } else {
                    die("!ERROR: %s: Include file not found: %s\n", parent_function, filename);
                }
            } else {
                die("!ERROR: %s: Invalid #include directive: %s\n", parent_function, line);
            }
        } else 
        {
            includes = strcatf(&includes, "\n%s", line); //@leak
        }
    }

    return includes;
}

static
char *shader_preprocess(const char *src, const char *defines) { // must FREE() after use
    if (!src) return NULL;

    const char *desktop = "#version 330\n";
    const char *gles = "#version 300 es\n";
    
    char *processed_src = file_preprocess(src, "shaderlib/", "shader()");
    const char *glsl_version = ifdef(ems, gles, desktop);

    // detect GLSL version if set
    if (processed_src[0] == '#' && processed_src[1] == 'v') {
        die("!ERROR: shader with #version specified on it. we do not support this anymore.");
    }

    char *preamble = file_read("shaderlib/compat.gl", 0);

    // add defines for configs
    static char *configs = NULL;
        FREE(configs);
        configs = stringf("#define MAX_LIGHTS %d\n#define MAX_SHADOW_LIGHTS %d\n#define NUM_SHADOW_CASCADES %d\n", MAX_LIGHTS, MAX_SHADOW_LIGHTS, NUM_SHADOW_CASCADES);

    static char *built = NULL;
        // FREE(built); // @fixme: uncomment
        built = stringf("%s\n%s\n%s\n%s\n%s", glsl_version, preamble ? preamble : "", configs ? configs : "", defines ? defines : "", processed_src);

    FREE(processed_src);
    return built;
}

unsigned shader_geom(const char *gs, const char *vs, const char *fs, const char *attribs, const char *fragcolor, const char *defines) {
#if 0
    // PRINTF(/*"!"*/"Compiling shader\n");

    static char *glsl_defines = 0;
    glsl_defines = REALLOC(glsl_defines, 1);
    glsl_defines[0] = 0;
    if( defines ) {
        for each_substring(defines, ",", def) {
            strcatf(&glsl_defines, "#define %s\n", def);
        }
    }

    if(gs)
    gs = shader_preprocess(gs, glsl_defines);
    vs = shader_preprocess(vs, glsl_defines);
    fs = shader_preprocess(fs, glsl_defines);

    GLuint vert = shader_compile(GL_VERTEX_SHADER, vs);
    GLuint frag = shader_compile(GL_FRAGMENT_SHADER, fs);
    GLuint geom = 0; if (gs) geom = shader_compile(GL_GEOMETRY_SHADER, gs);
    GLuint program = 0;

    if( vert && frag ) {
        program = glCreateProgram();

        glAttachShader(program, vert);
        glAttachShader(program, frag);
        if (geom) glAttachShader(program, geom);

        for( int i = 0; attribs && attribs[0]; ++i ) {
            char attrib[128] = {0};
            sscanf(attribs, "%127[^,]", attrib);
            while( attribs[0] && attribs[0] != ',' ) { attribs++; }
            while( attribs[0] && attribs[0] == ',' ) { attribs++; break; }
            if(!attrib[0]) continue;
            glBindAttribLocation(program, i, attrib);
            // PRINTF("Shader.attribute[%d]=%s\n", i, attrib);
        }

#if !is(ems) // @fixme
        if(fragcolor)
        glBindFragDataLocation(program, 0, fragcolor);
#endif

        glLinkProgram(program);

        GLint status = GL_FALSE, length;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
#ifdef DEBUG_SHADER
        if (status != GL_FALSE && program == DEBUG_SHADER) {
#else
        if (status == GL_FALSE) {
#endif
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
            // ASSERT(length < 2048); char buf[2048] = { 0 };
            char *buf = stack(length+1);
            glGetProgramInfoLog(program, length, NULL, buf);
            puts("--- vs:");
            shader_print(vs);
            puts("--- fs:");
            shader_print(fs);
            if (geom) {
                puts("--- gs:");
                shader_print(gs);
            }

            die("ERROR: shader():\nDevice: %s\nDriver: %s\nShader/program link: %s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION), buf);
        }

        glDeleteShader(vert);
        glDeleteShader(frag);
        if (geom) glDeleteShader(geom);

//#ifdef DEBUG_ANY_SHADER
//        PRINTF("Shader #%d:\n", program);
//        shader_print(vs);
//        shader_print(fs);
//#endif
    }

    glFinish();

/*
    if( s->program ) {
        strcatf(&s->name, "// vs (%s)\n%s\n\n\n", file_vs, vs);
        strcatf(&s->name, "// fs (%s)\n%s\n\n\n", file_fs, fs);
    }
*/

    // shader compiled fine, before returning, let's parse the source and reflect the uniforms
    array(char*) props = 0;
    do_once map_init_int( shader_reflect );
    if(vs) for each_substring(vs, "\r\n", line) {
        const char *found = strstr(line, "/""//");
        if( found > line && line[0] == '/' && line[1] == '/' ) continue;
        if( found ) array_push(props, STRDUP(line));
    }
    if(fs) for each_substring(fs, "\r\n", line) {
        const char *found = strstr(line, "/""//");
        if( found > line && line[0] == '/' && line[1] == '/' ) continue;
        if( found ) array_push(props, STRDUP(line));
    }
    if(gs) for each_substring(gs, "\r\n", line) {
        const char *found = strstr(line, "/""//");
        if( found > line && line[0] == '/' && line[1] == '/' ) continue;
        if( found ) array_push(props, STRDUP(line));
    }
    if( props ) {
        map_insert(shader_reflect, program, props);
        for( int i = 0; i < array_count(props); ++i ) shader_apply_param(program, i);
    }

#else

    shader_t s = shader2(vs, fs, attribs, fragcolor, defines, gs);
    unsigned program = s.program;

#endif

    return program;
}

unsigned shader_properties(unsigned shader) {
    array(char*) *found = map_find(shader_reflect, shader);
    return found ? array_count(*found) : 0;
}

char** shader_property(unsigned shader, unsigned property) {
    array(char*) *found = map_find(shader_reflect, shader);
    return found && property < array_count(*found) ? &(*found)[property] : NULL;
}

void shader_apply_param(unsigned shader, unsigned param_no) {
    unsigned num_properties = shader_properties(shader);
    if( param_no < num_properties ) {
        char *buf = *shader_property(shader, param_no);

        char type[32], name[32], line[256]; snprintf(line, 255, "%s", buf);
        if( sscanf(line, "%*s %s %[^ =;/]", type, name) != 2 ) return;

        char *mins = strstr(line, "min:");
        char *sets = strstr(line, "set:");
        char *maxs = strstr(line, "max:");
        char *tips = strstr(line, "tip:");
        if( mins ) *mins = 0, mins += 4;
        if( sets ) *sets = 0, sets += 4;
        if( maxs ) *maxs = 0, maxs += 4;
        if( tips ) *tips = 0, tips += 4;

        int is_color = !!strstri(name, "color"), top = is_color ? 1 : 10;
        vec4 minv = mins ? atof4(mins) : vec4(0,0,0,0);
        vec4 setv = sets ? atof4(sets) : vec4(0,0,0,0);
        vec4 maxv = maxs ? atof4(maxs) : vec4(top,top,top,top);

        if(minv.x > maxv.x) swapf(&minv.x, &maxv.x);
        if(minv.y > maxv.y) swapf(&minv.y, &maxv.y);
        if(minv.z > maxv.z) swapf(&minv.z, &maxv.z);
        if(minv.w > maxv.w) swapf(&minv.w, &maxv.w);

        if( !maxs ) {
        if(setv.x > maxv.x) maxv.x = setv.x;
        if(setv.y > maxv.y) maxv.y = setv.y;
        if(setv.z > maxv.z) maxv.z = setv.z;
        if(setv.w > maxv.w) maxv.w = setv.w;
        }

        setv = clamp4(setv, minv, maxv);

        if( strchr("ibfv", type[0]) ) {
            GLint shader_bak; glGetIntegerv(GL_CURRENT_PROGRAM, &shader_bak);
            glUseProgram(shader);
            /**/ if(type[0] == 'i') glUniform1i(glGetUniformLocation(shader, name), setv.x);
            else if(type[0] == 'b') glUniform1i(glGetUniformLocation(shader, name), !!setv.x);
            else if(type[0] == 'f') glUniform1f(glGetUniformLocation(shader, name), setv.x);
            else if(type[3] == '2') glUniform2fv(glGetUniformLocation(shader, name), 1, &setv.x);
            else if(type[3] == '3') glUniform3fv(glGetUniformLocation(shader, name), 1, &setv.x);
            else if(type[3] == '4') glUniform4fv(glGetUniformLocation(shader, name), 1, &setv.x);
            glUseProgram(shader_bak);
        }
    }
}

void shader_apply_params(unsigned shader, const char *parameter_mask) {
    unsigned num_properties = shader_properties(shader);
    for( unsigned i = 0; i < num_properties; ++i ) {
        char *line = *shader_property(shader,i);

        char name[32];
        if( sscanf(line, "%*s %*s %s", name) != 1 ) continue;
        if( !strmatch(name, parameter_mask) ) continue;
        shader_apply_param(shader, i);
    }
}

int ui_shader(unsigned shader) {
    int changed = 0;

    unsigned num_properties = shader_properties(shader);
    for( unsigned i = 0; i < num_properties; ++i ) {
        char **ptr = shader_property(shader,i);

        char line[256]; snprintf(line, 255, "%s", *ptr); // debug: ui_label(line);

        char uniform[32], type[32], name[32], early_exit = '\0';
        if( sscanf(line, "%s %s %[^ =;/]", uniform, type, name) != 3 ) continue; // @todo optimize: move to shader()

        char *mins = strstr(line, "min:");
        char *sets = strstr(line, "set:");
        char *maxs = strstr(line, "max:");
        char *tips = strstr(line, "tip:");
        if( mins ) *mins = 0, mins += 4;
        if( sets ) *sets = 0, sets += 4;
        if( maxs ) *maxs = 0, maxs += 4;
        if( tips ) *tips = 0, tips += 4;

        if( strcmp(uniform, "uniform") && strcmp(uniform, "}uniform") ) { if(tips) ui_label(va(UI_ICON(INFO) "%s", tips)); continue; } // @todo optimize: move to shader()

        int is_color = !!strstri(name, "color"), top = is_color ? 1 : 10;
        vec4 minv = mins ? atof4(mins) : vec4(0,0,0,0);
        vec4 setv = sets ? atof4(sets) : vec4(0,0,0,0);
        vec4 maxv = maxs ? atof4(maxs) : vec4(top,top,top,top);
        char *label = !tips ? va("%c%s", name[0] - 32 * !!(name[0] >= 'a'), name+1) :
            va("%c%s  " UI_ICON(INFO)  "@%s", name[0] - 32 * !!(name[0] >= 'a'), name+1, tips);

        if(minv.x > maxv.x) swapf(&minv.x, &maxv.x); // @optimize: move to shader()
        if(minv.y > maxv.y) swapf(&minv.y, &maxv.y); // @optimize: move to shader()
        if(minv.z > maxv.z) swapf(&minv.z, &maxv.z); // @optimize: move to shader()
        if(minv.w > maxv.w) swapf(&minv.w, &maxv.w); // @optimize: move to shader()

        if( !maxs ) {
        if(setv.x > maxv.x) maxv.x = setv.x;
        if(setv.y > maxv.y) maxv.y = setv.y;
        if(setv.z > maxv.z) maxv.z = setv.z;
        if(setv.w > maxv.w) maxv.w = setv.w;
        }

        setv = clamp4(setv, minv, maxv);

        // supports int,float,vec2/3/4,color3/4
        int touched = 0;
        if( type[0] == 'b' ) {
            bool v = !!setv.x;

            if( (touched = ui_bool(label, &v)) != 0 ) {
                setv.x = v;
            }
        }
        else if( type[0] == 'i' ) {
            int v = setv.x;

            if( (touched = ui_int(label, &v)) != 0 ) {
                setv.x = clampi(v, minv.x, maxv.x); // min..max range
            }
        }
        else if( type[0] == 'f' ) {
            setv.x = clampf(setv.x, minv.x, maxv.x);
            char *caption = va("%5.3f", setv.x);
            setv.x = (setv.x - minv.x) / (maxv.x - minv.x);

            if( (touched = ui_slider2(label, &setv.x, caption)) != 0 ) {
                setv.x = clampf(minv.x + setv.x * (maxv.x-minv.x), minv.x, maxv.x); // min..max range
            }
        }
        else if( type[0] == 'v' && type[3] == '2' ) {
            setv.xy = clamp2(setv.xy,minv.xy,maxv.xy);

            if( (touched = ui_float2(label, &setv.x)) != 0 ) {
                setv.xy = clamp2(setv.xy,minv.xy,maxv.xy);
            }
        }
        else if( type[0] == 'v' && type[3] == '3' ) {
            setv.xyz = clamp3(setv.xyz,minv.xyz,maxv.xyz);

            if( (touched = (is_color ? ui_color3f : ui_float3)(label, &setv.x)) != 0 ) {
                setv.xyz = clamp3(setv.xyz,minv.xyz,maxv.xyz);
            }
        }
        else if( type[0] == 'v' && type[3] == '4' ) {
            setv = clamp4(setv,minv,maxv);

            if( (touched = (is_color ? ui_color4f : ui_float4)(label, &setv.x)) != 0 ) {
                setv = clamp4(setv,minv,maxv);
            }
        }
        else if( tips ) ui_label( tips );

        if( touched ) {
            // upgrade value
            FREE(*ptr);
            *ptr = stringf("%s %s %s ///set:%s min:%s max:%s tip:%s", uniform,type,name,ftoa4(setv),ftoa4(minv),ftoa4(maxv),tips?tips:"");

            // apply
            shader_apply_param(shader, i);

            changed = 1;
        }
    }

    return changed;
}

int ui_shaders() {
    if( !map_count(shader_reflect) ) return ui_label(UI_ICON(WARNING) " No shaders with annotations loaded."), 0;

    int changed = 0;
    for each_map_ptr(shader_reflect, unsigned, k, array(char*), v) {
        int open = 0, clicked_or_toggled = 0;
        char *id = va("##SHD%d", *k);
        char *title = va("Shader %d", *k);
        for( int open = ui_collapse(title, id); open; ui_collapse_end(), open = 0) {
            ui_label(va("Shader %d",*k));
            changed |= ui_shader(*k);
        }
    }
    return changed;
}

#endif
