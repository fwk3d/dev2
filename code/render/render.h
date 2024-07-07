// -----------------------------------------------------------------------------
// (less) naive rendering framework
// - rlyeh, public domain
//
// IQM skeletal meshes by @lsalzman (public domain) - https://bit.ly/2OQh0Me
// SH code by @ands (public domain) - https://github.com/ands/spherical_harmonics_playground
// SHM code by @jarikomppa (unlicensed) - https://github.com/jarikomppa/shadertoolkit

#ifdef  M_PI
#undef  M_PI
#endif
#define M_PI (3.14159265358979323846264338327950288)

#if !CODE
typedef enum OBJTYPE_render {
    /// --
    OBJTYPE_sprite = 21,
    OBJTYPE_light  = 22,
} OBJTYPE_render;
#endif

#include "render_config.h" // first
#include "render_texture.h"
#include "render_texture_blit.h"
#include "render_texture_brdf.h"
#include "render_texture_rec.h"
#include "render_shader.h"
#include "render_shader2.h"
#include "render_shader_bind.h"
#include "render_shader_compute.h" // after shader
#include "render_shader_compute_ubo.h"
#include "render_shader_compute_ssbo.h"

#include "render_anim.h"
#include "render_assert.h"
#include "render_color.h"
#include "render_colormap.h"
#include "render_cubemap.h"
#include "render_ddraw.h"
#include "render_fbo.h"
#include "render_hash.h"
#include "render_material.h"
#include "render_mesh.h"
#include "render_occlusion.h"
#include "render_quad.h"
#include "render_rs.h"
#include "render_screenshot.h"
#include "render_skybox.h"
#include "render_ui.h"
#include "render_viewport.h"


#if 1
#include "render_sprite.h"
#include "render_sprite_tiled.h"
#include "render_shadertoy.h"
#include "render_postfx.h" // last_shader,ui_disable,
#include "render_sprite2.h"
#include "render_font.h" // stbtt

#include "render_light.h" // requires stream_draw>compute
#include "render_light_shadowmap.h" // requires render_pass > model
#include "render_model_uniform.h"
#include "render_model_drawmat.h" // requires model_t
#include "render_model.h" // requires light_t + model_uniform_t + model_drawmat_t
#endif

API uint64_t render_frame();
API float    render_fps();
API int      render_msaa();

#if CODE

uint64_t frame_count;

uint64_t render_frame() {
    return frame_count;
}
float render_fps() {
    return igGetIO()->Framerate;
}
int render_msaa() {
    return app_msaa;
}


void initR() {
	ddraw_init();
    sprite_init();
}
void tickR() {

}
void drawR() { // at begin frame. after swap>tick>[*]>draw
    // begin frame
    if (GLOBAL_FX_PASS_ENABLED && !postfx_backbuffer_draw) {
        if (!postfx_begin(&fx, window_width(), window_height())) {
            fbo_unbind();
        }
    }

    postfx_backbuffer_draw = false;
}
void swapR() { // at end frame. after tick>draw>[*]>swap
    // end frame
    if( frame_count > 0 ) {
        if (GLOBAL_FX_PASS_ENABLED && !postfx_backbuffer_draw) {
            postfx_end(&fx, 0, 0);
        }
    }

    ++frame_count;

    // flush batching systems that need to be rendered before frame swapping. order matters.
#if 0
    font_goto(0,0);
    touch_flush();
#endif
    sprite_flush();

    // flush all debugdraw calls before swap
    dd_ontop = 0;
    ddraw_flush();
    glClear(GL_DEPTH_BUFFER_BIT);
    dd_ontop = 1;
    ddraw_flush();

    // ui_render();

#if 0
#if !is(ems)
    // save screenshot if queued
    if( screenshot_file[0] ) {
        int n = 3;
        void *rgb = screenshot(n);
        stbi_flip_vertically_on_write(true);
        if(!stbi_write_png(screenshot_file, w, h, n, rgb, n * w) ) {
            die("!could not write screenshot file `%s`\n", screenshot_file);
        }
        screenshot_file[0] = 0;
    }
    if( record_active() ) {
        void record_frame();
        record_frame();
    }
#endif
#endif
}
void quitR() {

}

AUTORUN {
	events("init,draw,swap",initR,drawR,swapR);
}

#endif
