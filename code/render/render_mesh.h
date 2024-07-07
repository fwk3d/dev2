// -----------------------------------------------------------------------------
// meshes

#if !CODE

enum MESH_FLAGS {
    MESH_STATIC = 0, // STATIC, DYNAMIC, STREAM // zero|single|many updates per frame
    MESH_STREAM = 1,
    MESH_TRIANGLE_STRIP = 2,
};

typedef struct mesh_t {
    handle vao, vbo, ibo;
    unsigned vertex_count;
    unsigned index_count;
    unsigned flags;

    array(int) lod_collapse_map; // to which neighbor each vertex collapses. ie, [10] -> 7 (used by LODs) @leak

    // @leaks: following members are totally unused. convenient for end-users to keep their custom datas somewhere while processing.
    union {
    array(unsigned) in_index;
    array(vec3i)    in_index3;
    };
    union {
    array(unsigned) out_index;
    array(vec3i)    out_index3;
    };
    union {
    array(float) in_vertex;
    array(vec3) in_vertex3;
    };
    union {
    array(float) out_vertex;
    array(vec3) out_vertex3;
    };
} mesh_t;

API mesh_t mesh();
API   void mesh_update(mesh_t *m, const char *format, int vertex_stride,int vertex_count,const void *interleaved_vertex_data, int index_count,const void *index_data, int flags);
API   void mesh_render(mesh_t *m);
API   void mesh_render_prim(mesh_t *sm, unsigned prim);
API   void mesh_destroy(mesh_t *m);
API   aabb mesh_bounds(mesh_t *m);

#else

// -----------------------------------------------------------------------------
// meshes

mesh_t mesh() {
    mesh_t z = {0};
    return z;
}

aabb mesh_bounds(mesh_t *m) {
    aabb b = {{1e9,1e9,1e9},{-1e9,-1e9,-1e9}};
    for( int i = 0; i < array_count(m->in_vertex3); ++i ) {
        if( m->in_vertex3[i].x < b.min.x ) b.min.x = m->in_vertex3[i].x;
        if( m->in_vertex3[i].x > b.max.x ) b.max.x = m->in_vertex3[i].x;

        if( m->in_vertex3[i].y < b.min.y ) b.min.y = m->in_vertex3[i].y;
        if( m->in_vertex3[i].y > b.max.y ) b.max.y = m->in_vertex3[i].y;

        if( m->in_vertex3[i].z < b.min.z ) b.min.z = m->in_vertex3[i].z;
        if( m->in_vertex3[i].z > b.max.z ) b.max.z = m->in_vertex3[i].z;
    }
    return b;
}

void mesh_update(mesh_t *m, const char *format, int vertex_stride,int vertex_count,const void *vertex_data, int index_count,const void *index_data, int flags) {
    m->flags = flags;

    // setup
    unsigned sizeof_index = sizeof(GLuint);
    unsigned sizeof_vertex = 0;
    m->index_count = index_count;
    m->vertex_count = vertex_count;

    // iterate vertex attributes { position, normal + uv + tangent + bitangent + ... }
    struct vertex_descriptor {
        int vertex_type, num_attribute, num_components, alt_normalized;
        int stride, offset;
    } descriptor[16] = {0}, *dc = &descriptor[0];

    do switch( *format ) {
        break; case '*': dc->alt_normalized = 1;
        break; case '0': dc->num_components = 0;
        break; case '1': dc->num_components = 1;
        break; case '2': dc->num_components = 2;
        break; case '3': dc->num_components = 3;
        break; case '4': dc->num_components = 4;
        break; case 'F': dc->vertex_type = GL_FLOAT;
        break; case 'U': case 'I': dc->vertex_type = GL_UNSIGNED_INT;
        break; case 'B': if(format[-1] >= '0' && format[-1] <= '9') dc->vertex_type = GL_UNSIGNED_BYTE; //else bitangent.
        break; case ' ': while (format[1] == ' ') format++; case '\0':
            if (!dc->vertex_type) dc->vertex_type = GL_FLOAT;
            dc->offset = sizeof_vertex;
            sizeof_vertex += (dc->stride = dc->num_components * (dc->vertex_type == GL_UNSIGNED_BYTE ? 1 : 4));
            ++dc;
        break; default: if( !strchr("pntbcwai", *format) ) die("unsupported vertex type '%c'", *format);
    } while (*format++);

    if(vertex_stride > 0) sizeof_vertex = vertex_stride;

    // layout
    if(!m->vao) glGenVertexArrays(1, &m->vao);
    glBindVertexArray(m->vao);

    // index data
    if( index_data && index_count ) {
        m->index_count = index_count;

        if(!m->ibo) glGenBuffers(1, &m->ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->index_count * sizeof_index, index_data, flags & MESH_STREAM ? GL_STREAM_DRAW : GL_STATIC_DRAW);
    }

    // vertex data
    if( vertex_data && vertex_count ) {
        m->vertex_count = vertex_count;

        if(!m->vbo) glGenBuffers(1, &m->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
        glBufferData(GL_ARRAY_BUFFER, m->vertex_count * sizeof_vertex, vertex_data, flags & MESH_STREAM ? GL_STREAM_DRAW : GL_STATIC_DRAW);
    }

    for( int i = 0; i < 8; ++i ) {
//        glDisableVertexAttribArray(i);
    }

    // vertex setup: iterate descriptors
    for( int i = 0; i < countof(descriptor); ++i ) {
        if( descriptor[i].num_components ) {
            glDisableVertexAttribArray(i);
            glVertexAttribPointer(i,
                descriptor[i].num_components, descriptor[i].vertex_type, (descriptor[i].vertex_type == GL_UNSIGNED_BYTE ? GL_TRUE : GL_FALSE) ^ (descriptor[i].alt_normalized ? GL_TRUE : GL_FALSE),
                sizeof_vertex, (GLchar*)NULL + descriptor[i].offset);
            glEnableVertexAttribArray(i);
        } else {
            glDisableVertexAttribArray(i);
        }
    }

    glBindVertexArray(0);
}

void mesh_render(mesh_t *sm) {
    if( sm->vao ) {
        glBindVertexArray(sm->vao);
        if( sm->ibo ) { // with indices
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sm->ibo); // <-- why intel?
            glDrawElements(sm->flags & MESH_TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, sm->index_count, GL_UNSIGNED_INT, (char*)0);
            profile_incstat("Render.num_drawcalls", +1);
            profile_incstat("Render.num_triangles", sm->index_count/3);
        } else { // with vertices only
            glDrawArrays(sm->flags & MESH_TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, 0, sm->vertex_count /* / 3 */);
            profile_incstat("Render.num_drawcalls", +1);
            profile_incstat("Render.num_triangles", sm->vertex_count/3);
        }
    }
}

void mesh_render_prim(mesh_t *sm, unsigned prim) {
    if( sm->vao ) {
        glBindVertexArray(sm->vao);
        if( sm->ibo ) { // with indices
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sm->ibo); // <-- why intel?
            glDrawElements(prim, sm->index_count, GL_UNSIGNED_INT, (char*)0);
            profile_incstat("Render.num_drawcalls", +1);
            profile_incstat("Render.num_triangles", sm->index_count/3);
        } else { // with vertices only
            glDrawArrays(prim, 0, sm->vertex_count /* / 3 */);
            profile_incstat("Render.num_drawcalls", +1);
            profile_incstat("Render.num_triangles", sm->vertex_count/3);
        }
    }
}

void mesh_destroy(mesh_t *m) {
    // @todo
    glDeleteBuffers(1, &m->vbo);
    glDeleteBuffers(1, &m->ibo);
    glDeleteVertexArrays(1, &m->vao);
}

#endif
