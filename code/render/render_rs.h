// -----------------------------------------------------------------------------
// renderstate

#if !CODE

typedef struct renderstate_t {
    // Clear color
    float clear_color[4];

    // Color mask
    bool color_mask[4];

    // Clear depth
    double clear_depth;

    // Depth test
    bool depth_test_enabled;
    bool depth_write_enabled;
    unsigned depth_func;
    bool reverse_z;
    
    // Polygon offset
    bool polygon_offset_enabled;
    float polygon_offset;
    float polygon_offset_factor;

    // Blending
    bool blend_enabled;
    unsigned blend_func;
    unsigned blend_src;
    unsigned blend_dst;
    unsigned blend_src_alpha;
    unsigned blend_dst_alpha;

    // Culling
    bool cull_face_enabled;
    unsigned cull_face_mode;

    // Stencil test
    bool stencil_test_enabled;
    unsigned stencil_func;
    unsigned stencil_op_fail, stencil_op_zfail, stencil_op_zpass;
    int stencil_ref;
    unsigned stencil_read_mask;
    unsigned stencil_write_mask;

    // Face culling direction
    unsigned front_face; // GL_CW or GL_CCW

    // Line width
    bool line_smooth_enabled;
    float line_width;

    // Point size
    bool point_size_enabled;
    float point_size;

    // Polygon mode
    unsigned polygon_mode_face;
    unsigned polygon_mode_draw;

    // Scissor test
    bool scissor_test_enabled;

    // Seamless cubemap
    bool seamless_cubemap;

    // Depth clamp
    bool depth_clamp_enabled;
} renderstate_t;

API renderstate_t renderstate();
API bool            renderstate_compare(const renderstate_t *stateA, const renderstate_t *stateB);
API uint32_t        renderstate_checksum(const renderstate_t *state);
API void            renderstate_apply(const renderstate_t *state);

#else

// ----------------------------------------------------------------------------
// renderstate

renderstate_t renderstate() {
    renderstate_t state = {0};

    // Set default clear color to black
    state.clear_color[0] = 0.0f; // Red
    state.clear_color[1] = 0.0f; // Green
    state.clear_color[2] = 0.0f; // Blue
    state.clear_color[3] = 1.0f; // Alpha

    // Set default color mask to GL_TRUE
    state.color_mask[0] = GL_TRUE;
    state.color_mask[1] = GL_TRUE;
    state.color_mask[2] = GL_TRUE;
    state.color_mask[3] = GL_TRUE;

    // Set default clear depth to maximum distance
    state.clear_depth = 1.0;

    // Enable depth test by default with less or equal function
    state.depth_test_enabled = GL_TRUE;
    state.depth_write_enabled = GL_TRUE;

#if ENABLE_REVERSE_Z
    state.depth_func = GL_GREATER;
    state.reverse_z = gl_reversez ? 1 : 0;
#else
    state.depth_func = GL_LEQUAL;
    state.reverse_z = 0;
#endif
    
    // Disable polygon offset by default
    state.polygon_offset_enabled = GL_FALSE;
    state.polygon_offset_factor = 0.0f;
    state.polygon_offset = 0.0f;

    // Disable blending by default
    state.blend_enabled = GL_FALSE;
    state.blend_func = GL_FUNC_ADD;
    state.blend_src = GL_ONE;
    state.blend_dst = GL_ZERO;
    state.blend_src_alpha = GL_ONE;
    state.blend_dst_alpha = GL_ZERO;

    // Disable culling by default but cull back faces
    state.cull_face_enabled = GL_FALSE;
    state.cull_face_mode = GL_BACK;

    // Disable stencil test by default
    state.stencil_test_enabled = GL_FALSE;
    state.stencil_func = GL_ALWAYS;
    state.stencil_op_fail = GL_KEEP;
    state.stencil_op_zfail = GL_KEEP;
    state.stencil_op_zpass = GL_KEEP;
    state.stencil_ref = 0;
    state.stencil_read_mask = 0xFFFFFFFF;
    state.stencil_write_mask = 0xFFFFFFFF;

    // Set default front face to counter-clockwise
    state.front_face = GL_CCW;

    // Set default line width
    state.line_smooth_enabled = GL_FALSE;
    state.line_width = 1.0f;

    // Set default point size
    state.point_size_enabled = GL_FALSE;
    state.point_size = 1.0f;

    // Set default polygon mode to fill
    state.polygon_mode_face = GL_FRONT_AND_BACK;
    state.polygon_mode_draw = GL_FILL;

    // Disable scissor test by default
    state.scissor_test_enabled = GL_FALSE;

    // Enable seamless cubemap by default
    state.seamless_cubemap = GL_TRUE;

    // Disable depth clamp by default
    state.depth_clamp_enabled = GL_FALSE;

    return state;
}

bool renderstate_compare(const renderstate_t *stateA, const renderstate_t *stateB) {
    return memcmp(stateA, stateB, sizeof(renderstate_t)) == 0;
}

uint32_t renderstate_checksum(const renderstate_t *state) {
    return 0;//hash_bin(state, sizeof(renderstate_t));
}

static renderstate_t last_rs;

void renderstate_apply(const renderstate_t *state) {
    if (state != NULL) {
        // Compare renderstates and bail if they are the same
        if (renderstate_compare(state, &last_rs)) {
            return;
        }

        // Store renderstate
        last_rs = *state;

        // Apply clear color
        glClearColor(state->clear_color[0], state->clear_color[1], state->clear_color[2], state->clear_color[3]);

        // Apply color mask
        glColorMask(state->color_mask[0], state->color_mask[1], state->color_mask[2], state->color_mask[3]);

        // Apply clear depth (reverse z)
#if ENABLE_REVERSE_Z
        if (gl_reversez == 1) {
            if (state->reverse_z) {
                glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
            } else {
                glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
            }
            if (state->reverse_z) {
                glClearDepth(1.0f - state->clear_depth);
            } else {
                glClearDepth(state->clear_depth);
            }
        } else {
            glClearDepth(state->clear_depth);
        }
#else
        glClearDepth(state->clear_depth);
#endif

        // Apply depth test
        if (state->depth_test_enabled) {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(state->depth_func);
        } else {
            glDisable(GL_DEPTH_TEST);
        }

        // Apply polygon offset
        if (state->polygon_offset_enabled) {
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(state->polygon_offset_factor, state->polygon_offset);
        } else {
            glDisable(GL_POLYGON_OFFSET_FILL);
        }        

        // Apply depth write
        glDepthMask(state->depth_write_enabled);

        // Apply blending
        if (state->blend_enabled) {
            glEnable(GL_BLEND);
            glBlendEquation(state->blend_func);
            glBlendFunc(state->blend_src, state->blend_dst);
            //glBlendFuncSeparate(state->blend_src, state->blend_dst, state->blend_src_alpha, state->blend_dst_alpha);
        } else {
            glDisable(GL_BLEND);
        }

        // Apply culling @fixme
        if (state->cull_face_enabled) {
            glEnable(GL_CULL_FACE);
            glCullFace(state->cull_face_mode);
        } else {
            glDisable(GL_CULL_FACE);
        }

        // Apply stencil test
        if (state->stencil_test_enabled) {
            glEnable(GL_STENCIL_TEST);
            glStencilMask(state->stencil_write_mask);
            glStencilFunc(state->stencil_func, state->stencil_ref, state->stencil_read_mask);
            glStencilOp(state->stencil_op_fail, state->stencil_op_zfail, state->stencil_op_zpass);
        } else {
            glDisable(GL_STENCIL_TEST);
        }

        // Apply front face direction @fixme
        glFrontFace(state->front_face);

        // Apply line width
        glLineWidth(state->line_width);

        // Apply smooth lines
        if (state->line_smooth_enabled) {
            glEnable(GL_LINE_SMOOTH);
        } else {
            glDisable(GL_LINE_SMOOTH);
        }

#if !is(ems)
        // Apply point size
        if (state->point_size_enabled) {
            glEnable(GL_PROGRAM_POINT_SIZE);
            glPointSize(state->point_size);
        } else {
            glDisable(GL_PROGRAM_POINT_SIZE);
        }

        // Apply polygon mode
        glPolygonMode(state->polygon_mode_face, state->polygon_mode_draw);
#endif

        // Apply scissor test
        if (state->scissor_test_enabled) {
            glEnable(GL_SCISSOR_TEST);
        } else {
            glDisable(GL_SCISSOR_TEST);
        }

        // Apply seamless cubemap
        if (state->seamless_cubemap) {
            glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        } else {
            glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        }

        // Apply depth clamp
        if (state->depth_clamp_enabled) {
            glEnable(GL_DEPTH_CLAMP);
        } else {
            glDisable(GL_DEPTH_CLAMP);
        }
    }
}

#endif
