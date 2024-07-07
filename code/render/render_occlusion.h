// -----------------------------------------------------------------------------
// Occlusion queries

API unsigned query_test_point(mat44 proj, mat44 view, vec3 pos, float size);

#if CODE

// -----------------------------------------------------------------------------
// Occlusion queries

static renderstate_t query_test_rs;

static inline
unsigned query_adjust_samples_msaa(unsigned samples) {
    return samples / (render_msaa() + !render_msaa());
}

static inline
void query_test_rs_init() {
    do_once {
        query_test_rs = renderstate();
        query_test_rs.depth_test_enabled = true;
        query_test_rs.depth_write_enabled = false;
        query_test_rs.depth_func = GL_LESS;
        query_test_rs.point_size_enabled = 1;
        query_test_rs.point_size = 1.0f;
        memset(query_test_rs.color_mask, 0, sizeof(query_test_rs.color_mask));
    }
}

unsigned query_test_point(mat44 proj, mat44 view, vec3 pos, float size) {
    static int program = -1, vao = -1, u_mvp = -1, query = -1;
    if( program < 0 ) {
        const char* vs = file_read("shaders/query_point_vs.gl", 0);
        const char* fs = file_read("shaders/query_point_fs.gl", 0);

        program = shader(vs, fs, "", "fragcolor" , NULL);
        u_mvp = glGetUniformLocation(program, "u_mvp");
        glGenVertexArrays( 1, (GLuint*)&vao );
        glGenQueries(1, (GLuint*)&query);
        query_test_rs_init();
    }

    query_test_rs.point_size = size;
    renderstate_apply(&query_test_rs);

#if 1 //< @r-lyeh. use this?
    GLint oldprog = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &oldprog);
#else
    int oldprog = last_shader;
#endif
    glUseProgram( program );
    
    mat44 M; translation44(M, pos.x, pos.y, pos.z);
    mat44 MVP; multiply44x3(MVP, proj, view, M);
    glUniformMatrix4fv(u_mvp, 1, GL_FALSE, MVP);

    glBindVertexArray( vao );

    glBeginQuery(GL_SAMPLES_PASSED, query);
    glDrawArrays( GL_POINTS, 0, 1 );
    glEndQuery(GL_SAMPLES_PASSED);

    GLuint samples_passed = 0;
    glGetQueryObjectuiv(query, GL_QUERY_RESULT, &samples_passed);

    glBindVertexArray( 0 );
    glUseProgram( oldprog );
    return query_adjust_samples_msaa(samples_passed);
}

#endif
