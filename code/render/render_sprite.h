#if !CODE

// -----------------------------------------------------------------------------
// sprites

typedef enum SPRITE_FLAGS {
    SPRITE_PROJECTED = 1,
    SPRITE_ADDITIVE = 2,
    SPRITE_CENTERED = 4,
    SPRITE_RESOLUTION_INDEPENDANT = 128,
} SPRITE_FLAGS;

// texture id, position(x,y,depth sort), tint color, rotation angle
API void sprite( texture_t texture, float position[3], float rotation /*0*/, unsigned color /*~0u*/, unsigned flags);

// texture id, rect(x,y,w,h) is [0..1] normalized, then: pos(xyz,z-index), (scale.xy,offset.xy), rotation (degrees), color (rgba)
API void sprite_rect( texture_t t, vec4 rect, vec4 pos, vec4 scaleoff, float tilt_deg, unsigned tint_rgba, unsigned flags);

// texture id, sheet(frameNumber,X,Y) (frame in a X*Y spritesheet), position(x,y,depth sort), rotation angle, offset(x,y), scale(x,y), is_additive, tint color
API void sprite_sheet( texture_t texture, float sheet[3], float position[3], float rotation, float offset[2], float scale[2], unsigned rgba, unsigned flags);

API void sprite_flush();

// -----------------------------------------------------------------------------
// tilemaps

typedef struct tileset_t {
    texture_t tex;            // spritesheet
    unsigned tile_w, tile_h;  // dimensions per tile in pixels
    unsigned cols, rows;      // tileset num_cols, num_rows
    unsigned selected;        // active tile (while editing)
} tileset_t;

API tileset_t tileset(texture_t tex, unsigned tile_w, unsigned tile_h, unsigned cols, unsigned rows);

API int       ui_tileset( tileset_t t );

typedef struct tilemap_t {
    int blank_chr;                // transparent tile
    unsigned cols, rows;          // map dimensions (in tiles)
    array(int) map;

    vec3 position;                // x,y,scale
    float zindex;
    float tilt;
    unsigned tint;
    bool is_additive;
} tilemap_t;

API tilemap_t tilemap(const char *map, int blank_chr, int linefeed_chr);
API void      tilemap_render( tilemap_t m, tileset_t style );
API void      tilemap_render_ext( tilemap_t m, tileset_t style, float zindex, float xy_zoom[3], float tilt, unsigned tint, bool is_additive );

#else

// ----------------------------------------------------------------------------
// sprites

typedef struct sprite_static_t {
    float px, py, pz;         // origin x, y, depth
    float ox, oy, cos, sin;   // offset x, offset y, cos/sin of rotation degree
    float sx, sy;             // scale x,y
    float cellw, cellh;       // dimensions of any cell in spritesheet

    union {
    struct {
        int frame, ncx, ncy;      // frame in a (num cellx, num celly) spritesheet
    };
    struct {
        float x, y, w, h;         // normalized[0..1] within texture bounds
    };
    };

    uint32_t rgba, flags;     // vertex color and flags
} sprite_static_t;

// sprite batching
typedef struct batch_t { array(sprite_static_t) sprites; mesh_t mesh; int dirty; } batch_t;
typedef map(int, batch_t) batch_group_t; // mapkey is anything that forces a flush. texture_id for now, might be texture_id+program_id soon

// sprite stream
typedef struct sprite_vertex { vec3 pos; vec2 uv; uint32_t rgba; } sprite_vertex;
typedef struct sprite_index  { GLuint triangle[3]; } sprite_index;

#define sprite_vertex(...) CAST(sprite_vertex, __VA_ARGS__)
#define sprite_index(...)  CAST(sprite_index, __VA_ARGS__)

// sprite impl
static int sprite_count = 0;
static int sprite_program = -1;
static array(sprite_index)  sprite_indices = 0;
static array(sprite_vertex) sprite_vertices = 0;

// center_wh << 2 | additive << 1 | projected << 0
static batch_group_t sprite_group[8] = {0};

// rect(x,y,w,h) is [0..1] normalized, pos(xyz,z-index), scale_offset(sx,sy,offx,offy), rotation (degrees), color (rgba)
void sprite_rect( texture_t t, vec4 rect, vec4 pos, vec4 scale_offset, float tilt_deg, unsigned tint_rgba, unsigned flags) {
    float zindex = pos.w;
    float scalex = scale_offset.x;
    float scaley = scale_offset.y;
    float offsetx = scale_offset.z;
    float offsety = scale_offset.w;

    // do not queue if either scales or alpha are zero
    if( 0 == (scalex * scaley * ((tint_rgba>>24) & 255)) ) return;

    ASSERT( (flags & SPRITE_CENTERED) == 0 );
    if( flags & SPRITE_PROJECTED ) {
        tilt_deg += 180, scalex = -scalex; // flip texture Y on mvp3d (same than turn 180º then flip X)
    }

    sprite_static_t s = {0};

    s.px = pos.x, s.py = pos.y, s.pz = pos.z - zindex;
    s.sx = scalex, s.sy = scaley;

    s.x = rect.x, s.y = rect.y, s.w = rect.z, s.h = rect.w;
    s.cellw = s.w * s.sx * t.w, s.cellh = s.h * s.sy * t.h;

    s.rgba = tint_rgba;
    s.flags = flags;

#if 0
    s.ox = 0/*ox*/ * s.sx;
    s.oy = 0/*oy*/ * s.sy;
#else
    s.ox += offsetx * scalex;
    s.oy += offsety * scaley;
#endif

    if( tilt_deg ) {
        tilt_deg = (tilt_deg + 0) * ((float)C_PI / 180);
        s.cos = cosf(tilt_deg);
        s.sin = sinf(tilt_deg);
    } else {
        s.cos = 1;
        s.sin = 0;
    }

    batch_group_t *batches = &sprite_group[ flags & 7 ];
    batch_t *found = map_find_or_add(*batches, t.id, (batch_t){0});

    array_push(found->sprites, s);
}

void sprite_sheet( texture_t texture, float spritesheet[3], float position[3], float rotation, float offset[2], float scale[2], unsigned rgba, unsigned flags) {
    flags |= SPRITE_CENTERED;
    ASSERT( flags & SPRITE_CENTERED );

    const float px = position[0], py = position[1], pz = position[2];
    const float ox = offset[0], oy = offset[1], sx = scale[0], sy = scale[1];
    const float frame = spritesheet[0], xcells = spritesheet[1], ycells = spritesheet[2];

    if (frame < 0) return;
    if (frame > 0 && frame >= (xcells * ycells)) return;

    // no need to queue if alpha or scale are zero
    if( sx && sy && alpha(rgba) ) {
        vec3 bak = camera_get_active()->position;
        if( flags & SPRITE_RESOLUTION_INDEPENDANT ) { // @todo: optimize me
        sprite_flush();
        camera_get_active()->position = vec3(window_width()/2,window_height()/2,1);
        }

        sprite_static_t s;
        s.px = px;
        s.py = py;
        s.pz = pz;
        s.frame = frame;
        s.ncx = xcells ? xcells : 1;
        s.ncy = ycells ? ycells : 1;
        s.sx = sx;
        s.sy = sy;
        s.ox = ox * sx;
        s.oy = oy * sy;
        s.cellw = (texture.x * sx / s.ncx);
        s.cellh = (texture.y * sy / s.ncy);
        s.rgba = rgba;
        s.flags = flags;
        s.cos = 1;
        s.sin = 0;
        if(rotation) {
            rotation = (rotation + 0) * ((float)C_PI / 180);
            s.cos = cosf(rotation);
            s.sin = sinf(rotation);
        }

        batch_group_t *batches = &sprite_group[ flags & 7 ];
#if 0
        batch_t *found = map_find(*batches, texture.id);
        if( !found ) found = map_insert(*batches, texture.id, (batch_t){0});
#else
        batch_t *found = map_find_or_add(*batches, texture.id, (batch_t){0});
#endif

        array_push(found->sprites, s);

        if( flags & SPRITE_RESOLUTION_INDEPENDANT ) { // @todo: optimize me
        sprite_flush();
        camera_get_active()->position = bak;
        }
    }
}

void sprite( texture_t texture, float position[3], float rotation, unsigned color, unsigned flags) {
    float offset[2] = {0,0}, scale[2] = {1,1}, spritesheet[3] = {0,0,0};
    sprite_sheet( texture, spritesheet, position, rotation, offset, scale, color, flags );
}

static void sprite_rebuild_meshes() {
    sprite_count = 0;

    // w/2,h/2 centered
    for( int l = countof(sprite_group) / 2; l < countof(sprite_group); ++l) {
        for each_map_ptr(sprite_group[l], int,_, batch_t,bt) {

            bt->dirty = array_count(bt->sprites) ? 1 : 0;
            if( !bt->dirty ) continue;

            int index = 0;
            array_clear(sprite_indices);
            array_clear(sprite_vertices);

            array_foreach_ptr(bt->sprites, sprite_static_t,it ) {
                float x0 = it->ox - it->cellw/2, x3 = x0 + it->cellw;
                float y0 = it->oy - it->cellh/2, y3 = y0;
                float x1 = x0,                   x2 = x3;
                float y1 = y0 + it->cellh,       y2 = y1;

                // @todo: move this affine transform into glsl shader
                vec3 v0 = { it->px + ( x0 * it->cos - y0 * it->sin ), it->py + ( x0 * it->sin + y0 * it->cos ), it->pz };
                vec3 v1 = { it->px + ( x1 * it->cos - y1 * it->sin ), it->py + ( x1 * it->sin + y1 * it->cos ), it->pz };
                vec3 v2 = { it->px + ( x2 * it->cos - y2 * it->sin ), it->py + ( x2 * it->sin + y2 * it->cos ), it->pz };
                vec3 v3 = { it->px + ( x3 * it->cos - y3 * it->sin ), it->py + ( x3 * it->sin + y3 * it->cos ), it->pz };

                float cx = (1.0f / it->ncx) - 1e-9f;
                float cy = (1.0f / it->ncy) - 1e-9f;
                int idx = (int)it->frame;
                int px = idx % it->ncx;
                int py = idx / it->ncx;

                float ux = px * cx, uy = py * cy;
                float vx = ux + cx, vy = uy + cy;

                vec2 uv0 = vec2(ux, uy);
                vec2 uv1 = vec2(ux, vy);
                vec2 uv2 = vec2(vx, vy);
                vec2 uv3 = vec2(vx, uy);

                array_push( sprite_vertices, sprite_vertex(v0, uv0, it->rgba) ); // Vertex 0 (A)
                array_push( sprite_vertices, sprite_vertex(v1, uv1, it->rgba) ); // Vertex 1 (B)
                array_push( sprite_vertices, sprite_vertex(v2, uv2, it->rgba) ); // Vertex 2 (C)
                array_push( sprite_vertices, sprite_vertex(v3, uv3, it->rgba) ); // Vertex 3 (D)

                //      A--B                  A               A-B
                // quad |  | becomes triangle |\  and triangle \|
                //      D--C                  D-C               C
                GLuint A = (index+0), B = (index+1), C = (index+2), D = (index+3); index += 4;

                array_push( sprite_indices, sprite_index(C, D, A) ); // Triangle 1
                array_push( sprite_indices, sprite_index(C, A, B) ); // Triangle 2
            }

            mesh_update(&bt->mesh, "p3 t2 c4B", 0,array_count(sprite_vertices),sprite_vertices, 3*array_count(sprite_indices),sprite_indices, MESH_STATIC);

            // clear elements from queue
            sprite_count += array_count(bt->sprites);
            array_clear(bt->sprites);
        }
    }

    // (0,0) centered
    for( int l = 0; l < countof(sprite_group) / 2; ++l) {
        for each_map_ptr(sprite_group[l], int,_, batch_t,bt) {

            bt->dirty = array_count(bt->sprites) ? 1 : 0;
            if( !bt->dirty ) continue;

            int index = 0;
            array_clear(sprite_indices);
            array_clear(sprite_vertices);

            array_foreach_ptr(bt->sprites, sprite_static_t,it ) {
                float x0 = it->ox - it->cellw/2, x3 = x0 + it->cellw;
                float y0 = it->oy - it->cellh/2, y3 = y0;
                float x1 = x0,                   x2 = x3;
                float y1 = y0 + it->cellh,       y2 = y1;

                // @todo: move this affine transform into glsl shader
                vec3 v0 = { it->px + ( x0 * it->cos - y0 * it->sin ), it->py + ( x0 * it->sin + y0 * it->cos ), it->pz };
                vec3 v1 = { it->px + ( x1 * it->cos - y1 * it->sin ), it->py + ( x1 * it->sin + y1 * it->cos ), it->pz };
                vec3 v2 = { it->px + ( x2 * it->cos - y2 * it->sin ), it->py + ( x2 * it->sin + y2 * it->cos ), it->pz };
                vec3 v3 = { it->px + ( x3 * it->cos - y3 * it->sin ), it->py + ( x3 * it->sin + y3 * it->cos ), it->pz };

                float ux = it->x, vx = ux + it->w;
                float uy = it->y, vy = uy + it->h;

                vec2 uv0 = vec2(ux, uy);
                vec2 uv1 = vec2(ux, vy);
                vec2 uv2 = vec2(vx, vy);
                vec2 uv3 = vec2(vx, uy);

                array_push( sprite_vertices, sprite_vertex(v0, uv0, it->rgba) ); // Vertex 0 (A)
                array_push( sprite_vertices, sprite_vertex(v1, uv1, it->rgba) ); // Vertex 1 (B)
                array_push( sprite_vertices, sprite_vertex(v2, uv2, it->rgba) ); // Vertex 2 (C)
                array_push( sprite_vertices, sprite_vertex(v3, uv3, it->rgba) ); // Vertex 3 (D)

                //      A--B                  A               A-B
                // quad |  | becomes triangle |\  and triangle \|
                //      D--C                  D-C               C
                GLuint A = (index+0), B = (index+1), C = (index+2), D = (index+3); index += 4;

                array_push( sprite_indices, sprite_index(C, D, A) ); // Triangle 1
                array_push( sprite_indices, sprite_index(C, A, B) ); // Triangle 2
            }

            mesh_update(&bt->mesh, "p3 t2 c4B", 0,array_count(sprite_vertices),sprite_vertices, 3*array_count(sprite_indices),sprite_indices, MESH_STATIC);

            // clear elements from queue
            sprite_count += array_count(bt->sprites);
            array_clear(bt->sprites);
        }
    }
}

static void sprite_render_meshes_group(batch_group_t* sprites, int alpha_key, int alpha_value, float mvp[16]) {
    if( map_count(*sprites) > 0 ) {
        // setup shader
        if( sprite_program < 0 ) {
            sprite_program = shader( file_read("shaders/sprite_vs.gl", 0), file_read("shaders/sprite_fs.gl", 0),
                "att_Position,att_TexCoord,att_Color",
                "fragColor", NULL
            );
        }
        shader_bind(sprite_program);
        shader_mat44("u_mvp", mvp);

        // set (unit 0) in the uniform texture sampler, and render batch
        glActiveTexture(GL_TEXTURE0);
        glBlendFunc( alpha_key, alpha_value );

        for each_map_ptr(*sprites, int,texture_id, batch_t,bt) {
            if( bt->dirty ) {
                shader_texture("u_texture", *texture_id, 0);
                mesh_render(&bt->mesh);
            }
        }
//      map_clear(*sprites);
    }
}

static void sprite_init() {
    do_once for(int i = 0; i < countof(sprite_group); ++i) {
    map_init(sprite_group[i], less_int, hash_int);
    }
}

static renderstate_t sprite_rs;

void sprite_flush() {
    do_once {
        sprite_rs = renderstate();
        sprite_rs.reverse_z = 0;
        sprite_rs.depth_test_enabled = 1;
        sprite_rs.blend_enabled = 1;
        sprite_rs.cull_face_enabled = 0;
        sprite_rs.front_face = GL_CCW;
        sprite_rs.depth_func = GL_LEQUAL;
    }
    
    profile("Sprite.rebuild_time") {
        sprite_rebuild_meshes();
    }
    profile("Sprite.render_time") {
        // setup rendering state
        renderstate_apply(&sprite_rs);

        // 3d
        mat44 mvp3d; multiply44x2(mvp3d, camera_get_active()->proj, camera_get_active()->view);
        // render all additive then translucent groups
        sprite_render_meshes_group(&sprite_group[SPRITE_PROJECTED], GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, mvp3d );
        sprite_render_meshes_group(&sprite_group[SPRITE_PROJECTED|SPRITE_CENTERED], GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, mvp3d );
        sprite_render_meshes_group(&sprite_group[SPRITE_PROJECTED|SPRITE_CENTERED|SPRITE_ADDITIVE], GL_SRC_ALPHA, GL_ONE, mvp3d );
        sprite_render_meshes_group(&sprite_group[SPRITE_PROJECTED|SPRITE_ADDITIVE], GL_SRC_ALPHA, GL_ONE, mvp3d );

        // 2d: (0,0) is center of screen
        mat44 mvp2d;
        vec3 pos = camera_get_active()->position;
        float zoom = absf(pos.z); if(zoom < 0.1f) zoom = 0.1f; zoom = 1.f / (zoom + !zoom);
        float zdepth_max = window_height(); // 1;
        float l = pos.x - window_width()  * zoom / 2;
        float r = pos.x + window_width()  * zoom / 2;
        float b = pos.y + window_height() * zoom / 2;
        float t = pos.y - window_height() * zoom / 2;
        ortho44(mvp2d, l,r,b,t, -zdepth_max, +zdepth_max);
        // render all additive then translucent groups
        sprite_render_meshes_group(&sprite_group[0], GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, mvp2d );
        sprite_render_meshes_group(&sprite_group[SPRITE_CENTERED], GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, mvp2d );
        sprite_render_meshes_group(&sprite_group[SPRITE_CENTERED|SPRITE_ADDITIVE], GL_SRC_ALPHA, GL_ONE, mvp2d );
        sprite_render_meshes_group(&sprite_group[SPRITE_ADDITIVE], GL_SRC_ALPHA, GL_ONE, mvp2d );

        // restore rendering state
        glUseProgram(0);
    }
}

// -----------------------------------------------------------------------------
// tilemaps

tilemap_t tilemap(const char *map, int blank_chr, int linefeed_chr) {
    tilemap_t t = {0};
    t.tint = ~0u; // WHITE
    t.blank_chr = blank_chr;
    for( ; *map ; ++map ) {
        if( map[0] == linefeed_chr ) ++t.rows;
        else {
            array_push(t.map, map[0]);
            ++t.cols;
        }
    }
    return t;
}

void tilemap_render_ext( tilemap_t m, tileset_t t, float zindex, float xy_zoom[3], float tilt, unsigned tint, bool is_additive ) {
    vec3 old_pos = camera_get_active()->position;
    sprite_flush();
    camera_get_active()->position = vec3(window_width()/2,window_height()/2,1);

    float scale[2] = {xy_zoom[2], xy_zoom[2]};
    xy_zoom[2] = zindex;

    float offset[2] = {0,0};
    float spritesheet[3] = {0,t.cols,t.rows}; // selected tile index and spritesheet dimensions (cols,rows)

    for( unsigned y = 0, c = 0; y < m.rows; ++y ) {
        for( unsigned x = 0; x < m.cols; ++x, ++c ) {
            if( m.map[c] != m.blank_chr ) {
                spritesheet[0] = m.map[c];
                sprite_sheet(t.tex, spritesheet, xy_zoom, tilt, offset, scale, tint, is_additive ? SPRITE_ADDITIVE : 0);
            }
            offset[0] += t.tile_w;
        }
        offset[0] = 0, offset[1] += t.tile_h;
    }

    sprite_flush();
    camera_get_active()->position = old_pos;
}

void tilemap_render( tilemap_t map, tileset_t set ) {
    map.position.x += set.tile_w;
    map.position.y += set.tile_h;
    tilemap_render_ext( map, set, map.zindex, &map.position.x, map.tilt, map.tint, map.is_additive );
}

tileset_t tileset(texture_t tex, unsigned tile_w, unsigned tile_h, unsigned cols, unsigned rows) {
    tileset_t t = {0};
    t.tex = tex;
    t.cols = cols, t.rows = rows;
    t.tile_w = tile_w, t.tile_h = tile_h;
    return t;
}

int ui_tileset( tileset_t t ) {
    vec2 uv0 = vec2((t.selected % t.cols) * t.tile_w, (t.selected / t.cols) * t.tile_h);
    vec2 uv1 = add2(uv0, vec2(t.tile_w, t.tile_h));
    ui_subimage(va("Selection #%d (%d,%d)", t.selected, t.selected % t.cols, t.selected / t.cols), 
        t.tex.id, t.tile_w, t.tile_h, uv0.x / t.tex.w, uv0.y / t.tex.h, uv1.x / t.tex.w, uv1.y / t.tex.h);
    unsigned choice;
    if( (choice = ui_image(0, t.tex.id, t.tex.w,t.tex.h)) ) {
        float px = (((choice >> 8) & 0xff) / 256.f) * t.tex.w;
        float py = (((choice >> 0) & 0xff) / 256.f) * t.tex.h;
        float tx = px / t.tile_w;
        float ty = py / t.tile_h;
        t.selected = tx + ty * t.cols;
    }
    // if( (choice = ui_buttons(3, "load", "save", "clear")) ) {}
    return t.selected;
}

// ----------------------------------------------------------------------------

// texture_t texture_createclip(unsigned cx,unsigned cy,unsigned cw,unsigned ch, unsigned tw,unsigned th,unsigned tn,void *pixels, unsigned flags) {
//     return texture_create(tw,th,tn,pixels,flags);
//     static array(unsigned) clip = 0;
//     array_resize(clip, cw*ch*4);
//     for( unsigned y = 0; y < ch; ++y )
//     memcpy((char *)clip + (0+(0+y)*cw)*tn, (char*)pixels + (cx+(cy+y)*tw)*tn, cw*tn);
//     return texture_create(cw,ch,tn,clip,flags);
// }

#if 0

char *trimspace(char *str) {
    for( char *s = str; *s; ++s )
        if(*s <= 32) memmove(s, s+1, strlen(s));
    return str;
}

char *file_parent(const char *f) {   // folder/folder/abc
    char *p = va("%s",file_path(f)); // folder/folder/
    char *last = strrchr(p, '/');    //              ^
    if( !last ) return p;            // return parent if no sep
    *last = '\0';                    // folder/folder
    last = strrchr(p, '/');          //       ^
    return last ? last + 1 : p;      // return parent if no sep
}
#endif

#endif
