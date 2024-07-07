static struct nk_context *ui_ctx;
static struct nk_glfw nk_glfw = {0};

void* ui_handle() {
    return ui_ctx;
}

static void nk_config_custom_fonts() {
    #define UI_ICON_MIN ICON_MD_MIN
    #define UI_ICON_MED ICON_MD_MAX_16
    #define UI_ICON_MAX ICON_MD_MAX

    #define ICON_BARS        ICON_MD_MENU
    #define ICON_FILE        ICON_MD_INSERT_DRIVE_FILE
    #define ICON_TRASH       ICON_MD_DELETE

    struct nk_font *font = NULL;
    struct nk_font_atlas *atlas = NULL;
    nk_glfw3_font_stash_begin(&nk_glfw, &atlas); // nk_sdl_font_stash_begin(&atlas);

        // Default font(#1)...
        int datalen = 0;
        for( char *data = file_read(UI_FONT_REGULAR, &datalen); data; data = 0 ) {
            float font_size = UI_FONT_REGULAR_SIZE;
                struct nk_font_config cfg = nk_font_config(font_size);
                cfg.oversample_h = UI_FONT_REGULAR_SAMPLING.x;
                cfg.oversample_v = UI_FONT_REGULAR_SAMPLING.y;
                cfg.pixel_snap   = UI_FONT_REGULAR_SAMPLING.z;
                #if UI_LESSER_SPACING
                cfg.spacing.x -= 1.0;
                #endif
            // win32: struct nk_font *arial = nk_font_atlas_add_from_file(atlas, va("%s/fonts/arial.ttf",getenv("windir")), font_size, &cfg); font = arial ? arial : font;
            // struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "nuklear/extra_font/DroidSans.ttf", font_size, &cfg); font = droid ? droid : font;
            struct nk_font *regular = nk_font_atlas_add_from_memory(atlas, data, datalen, font_size, &cfg); font = regular ? regular : font;
        }

        // ...with icons embedded on it.
        static struct icon_font {
            const char *file; int yspacing; vec3 sampling; nk_rune range[3];
        } icons[] = {
            {"MaterialIconsSharp-Regular.otf", UI_ICON_SPACING_Y, {1,1,1}, {UI_ICON_MIN, UI_ICON_MED /*MAX*/, 0}}, // "MaterialIconsOutlined-Regular.otf" "MaterialIcons-Regular.ttf"
            {"materialdesignicons-webfont.ttf", 2, {1,1,1}, {0xF68C /*ICON_MDI_MIN*/, 0xF1CC7/*ICON_MDI_MAX*/, 0}},
        };
        for( int f = 0; f < COUNTOF(icons); ++f )
        for( char *data = file_read(icons[f].file, &datalen); data; data = 0 ) {
            struct nk_font_config cfg = nk_font_config(UI_ICON_FONTSIZE);
            cfg.range = icons[f].range; // nk_font_default_glyph_ranges();
            cfg.merge_mode = 1;

            cfg.spacing.x += UI_ICON_SPACING_X;
            cfg.spacing.y += icons[f].yspacing;
         // cfg.font->ascent += ICON_ASCENT;
         // cfg.font->height += ICON_HEIGHT;

            cfg.oversample_h = icons[f].sampling.x;
            cfg.oversample_v = icons[f].sampling.y;
            cfg.pixel_snap   = icons[f].sampling.z;

            #if UI_LESSER_SPACING
            cfg.spacing.x -= 1.0;
            #endif

            struct nk_font *icons = nk_font_atlas_add_from_memory(atlas, data, datalen, UI_ICON_FONTSIZE, &cfg);
        }

        // Monospaced font. Used in terminals or consoles.

        for( char *data = file_read(UI_FONT_TERMINAL, &datalen); data; data = 0 ) {
            const float font_size = UI_FONT_TERMINAL_SIZE;
            static const nk_rune icon_range[] = {32, 127, 0};

            struct nk_font_config cfg = nk_font_config(font_size);
            cfg.range = icon_range;

            cfg.oversample_h = UI_FONT_TERMINAL_SAMPLING.x;
            cfg.oversample_v = UI_FONT_TERMINAL_SAMPLING.y;
            cfg.pixel_snap   = UI_FONT_TERMINAL_SAMPLING.z;

            #if UI_LESSER_SPACING
            cfg.spacing.x -= 1.0;
            #endif

            // struct nk_font *proggy = nk_font_atlas_add_default(atlas, font_size, &cfg);
            struct nk_font *bold = nk_font_atlas_add_from_memory(atlas, data, datalen, font_size, &cfg);
        }

        // Extra optional fonts from here...

        for( char *data = file_read(UI_FONT_HEADING, &datalen); data; data = 0 ) {
            struct nk_font_config cfg = nk_font_config(UI_FONT_HEADING_SIZE);
            cfg.oversample_h = UI_FONT_HEADING_SAMPLING.x;
            cfg.oversample_v = UI_FONT_HEADING_SAMPLING.y;
            cfg.pixel_snap   = UI_FONT_HEADING_SAMPLING.z;

            #if UI_LESSER_SPACING
            cfg.spacing.x -= 1.0;
            #endif

            struct nk_font *bold = nk_font_atlas_add_from_memory(atlas, data, datalen, UI_FONT_HEADING_SIZE, &cfg);
            // font = bold ? bold : font;
        }

    nk_glfw3_font_stash_end(&nk_glfw); // nk_sdl_font_stash_end();
//  ASSERT(font);
    if(font) nk_style_set_font(ui_ctx, &font->handle);

    // Load Cursor: if you uncomment cursor loading please hide the cursor
    // nk_style_load_all_cursors(ctx, atlas->cursors); glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}


int ui_active() {
    return ui_is_active || postfx_debug_tool_enabled; //window_has_cursor() && nk_window_is_any_hovered(ui_ctx) && nk_item_is_any_active(ui_ctx);
}


static unsigned ui_collapse_state = 0;
int ui_collapse(const char *label, const char *id) { // mask: 0(closed),1(open),2(created)
    int start_open = label[0] == '!'; label += start_open;

    uint64_t hash = 14695981039346656037ULL, mult = 0x100000001b3ULL;
    for(int i = 0; id[i]; ++i) hash = (hash ^ id[i]) * mult;
    ui_hue = (hash & 0x3F) / (float)0x3F; ui_hue += !ui_hue;

    int forced = ui_filter && ui_filter[0];
    enum nk_collapse_states forced_open = NK_MAXIMIZED;

    ui_collapse_state = nk_tree_base_(ui_ctx, NK_TREE_NODE, 0, label, start_open ? NK_MAXIMIZED : NK_MINIMIZED, forced ? &forced_open : NULL, id, strlen(id), 0);

    return ui_collapse_state & 1; // |1 open, |2 clicked, |4 toggled
}
int ui_collapse_clicked() {
    return ui_collapse_state >> 1; // |1 clicked, |2 toggled
}

// -----------------------------------------------------------------------------
// code for all the widgets

static
int nk_button_transparent(struct nk_context *ctx, const char *text) {
    static struct nk_style_button transparent_style;
    do_once transparent_style = ctx->style.button;
    do_once transparent_style.text_alignment = NK_TEXT_ALIGN_CENTERED|NK_TEXT_ALIGN_MIDDLE;
    do_once transparent_style.normal.data.color = nk_rgba(0,0,0,0);
    do_once transparent_style.border_color = nk_rgba(0,0,0,0);
    do_once transparent_style.active = transparent_style.normal;
    do_once transparent_style.hover = transparent_style.normal;
    do_once transparent_style.hover.data.color = nk_rgba(0,0,0,127);
    transparent_style.text_background.a = 255 * ui_alpha;
    transparent_style.text_normal.a = 255 * ui_alpha;
    transparent_style.text_hover.a = 255 * ui_alpha;
    transparent_style.text_active.a = 255 * ui_alpha;
    return nk_button_label_styled(ctx, &transparent_style, text);
}

// internal vars for our editor. @todo: maybe expose these to the end-user as well?
bool ui_label_icon_highlight;
vec2 ui_label_icon_clicked_L; // left
vec2 ui_label_icon_clicked_R; // right

static
int ui_label_(const char *label, int alignment) {
    // beware: assuming label can start with any ICON_MD_ glyph, which I consider them to be a 3-bytes utf8 sequence.
    // done for optimization reasons because this codepath is called a lot!
    const char *icon = label ? label : ""; while( icon[0] == '!' || icon[0] == '*' ) ++icon;
    int has_icon = (unsigned)icon[0] > 127, icon_len = 3, icon_width_px = 1*24;

    struct nk_rect bounds = nk_widget_bounds(ui_ctx);
    const struct nk_input *input = &ui_ctx->input;
    int is_hovering = nk_input_is_mouse_hovering_rect(input, bounds) && !ui_has_active_popups;
    if( is_hovering ) {
        struct nk_rect winbounds = nk_window_get_bounds(ui_ctx);
        is_hovering &= nk_input_is_mouse_hovering_rect(input, winbounds);

        struct nk_window *win = ui_ctx->current;
        bool has_contextual = !win->name; // contextual windows are annonymous

        is_hovering &= has_contextual || nk_window_has_focus(ui_ctx);
    }

    int skip_color_tab = label && label[0] == '!';
    if( skip_color_tab) label++;

    int spacing = 8; // between left colorbar and content
    struct nk_window *win = ui_ctx->current;
    struct nk_panel *layout = win->layout;
    layout->at_x += spacing;
    layout->bounds.w -= spacing;
    if( !skip_color_tab ) {
        float w = is_hovering ? 4 : 2; // spacing*3/4 : spacing/2-1;
        bounds.w = w;
        bounds.h -= 1;
        struct nk_command_buffer *canvas = nk_window_get_canvas(ui_ctx);
        nk_fill_rect(canvas, bounds, 0, nk_hsva_f(ui_hue, 0.75f, 0.8f, ui_alpha) );
    }

    if(!label || !label[0]) {
        nk_label(ui_ctx, "", alignment);
        layout->at_x -= spacing;
        layout->bounds.w += spacing;
        return 0;
    }

        const char *split = strchr(label, '@');
            char buffer[128]; if( split ) label = (snprintf(buffer, 128, "%.*s", (int)(split-label), label), buffer);

struct nk_style *style = &ui_ctx->style;
bool bold = label[0] == '*'; label += bold;
struct nk_font *font = bold && nk_glfw.atlas.fonts->next ? nk_glfw.atlas.fonts->next->next /*3rd font*/ : NULL; // list

if( !has_icon ) {
    // set bold style and color if needed
    if( font && nk_style_push_font(ui_ctx, &font->handle) ) {} else font = 0;
    if( font )  nk_style_push_color(ui_ctx, &style->text.color, nk_rgba(255, 255, 255, 255 * ui_alpha));
    nk_label(ui_ctx, label, alignment);
} else {
    char *icon_glyph = va("%.*s", icon_len, icon);

// @todo: implement nk_push_layout()
//  nk_rect bounds = {..}; nk_panel_alloc_space(bounds, ctx);
    struct nk_window *win = ui_ctx->current;
    struct nk_panel *layout = win->layout, copy = *layout;
    struct nk_rect before; nk_layout_peek(&before, ui_ctx);
    nk_label_colored(ui_ctx, icon_glyph, alignment, nk_rgba(255, 255, 255, (64 + 192 * ui_label_icon_highlight) * ui_alpha) );
    struct nk_rect after; nk_layout_peek(&after, ui_ctx);
    *layout = copy;
    layout->at_x += icon_width_px; layout->bounds.w -= icon_width_px; // nk_layout_space_push(ui_ctx, nk_rect(0,0,icon_width_px,0));

    // set bold style and color if needed
    if( font && nk_style_push_font(ui_ctx, &font->handle) ) {} else font = 0;
    if( font )  nk_style_push_color(ui_ctx, &style->text.color, nk_rgba(255, 255, 255, 255 * ui_alpha));
    nk_label(ui_ctx, icon+icon_len, alignment);

    layout->at_x -= icon_width_px; layout->bounds.w += icon_width_px;
}

if( font )  nk_style_pop_color(ui_ctx);
if( font )  nk_style_pop_font(ui_ctx);

            if (split && is_hovering && !ui_has_active_popups && nk_window_has_focus(ui_ctx)) {
                nk_tooltip(ui_ctx, split + 1); // @fixme: not working under ui_disable() state
            }

    layout->at_x -= spacing;
    layout->bounds.w += spacing;

    // old way
    // ui_labeicon_l_icked_L.x = is_hovering ? nk_input_has_mouse_click_down_in_rect(input, NK_BUTTON_LEFT, layout->bounds, nk_true) : 0;
    // new way
    // this is an ugly hack to detect which icon (within a label) we're clicking on.
    // @todo: figure out a better way to detect this... would it be better to have a ui_label_toolbar(lbl,bar) helper function instead?
    ui_label_icon_clicked_L.x = is_hovering ? ( (int)((input->mouse.pos.x - bounds.x) - (alignment == NK_TEXT_RIGHT ? bounds.w : 0) ) * nk_input_is_mouse_released(input, NK_BUTTON_LEFT)) : 0;

    return ui_label_icon_clicked_L.x;
}

int ui_label(const char *label) {
    if( label && ui_filter && ui_filter[0] ) if( !strstri(label, ui_filter) ) return 0;

    int align = label[0] == '>' ? (label++, NK_TEXT_RIGHT) : label[0] == '=' ? (label++, NK_TEXT_CENTERED) : label[0] == '<' ? (label++, NK_TEXT_LEFT) : NK_TEXT_LEFT;
    nk_layout_row_dynamic(ui_ctx, 0, 1);
    return ui_label_(label, align);
}

int ui_slider(const char *label, float *slider) {
    if( label && ui_filter && ui_filter[0] ) if( !strstri(label, ui_filter) ) return 0;

    // return ui_slider2(label, slider, va("%.2f ", *slider));
    nk_layout_row_dynamic(ui_ctx, 0, 2);
    ui_label_(label, NK_TEXT_LEFT);

    nk_size val = *slider * 1000;
    int chg = nk_progress(ui_ctx, &val, 1000, NK_MODIFIABLE);
    *slider = val / 1000.f;
    return chg;
}
int ui_slider2(const char *label, float *slider, const char *caption) {
    if( label && ui_filter && ui_filter[0] ) if( !strstri(label, ui_filter) ) return 0;

    nk_layout_row_dynamic(ui_ctx, 0, 2);
    ui_label_(label, NK_TEXT_LEFT);

    struct nk_window *win = ui_ctx->current;
    const struct nk_style *style = &ui_ctx->style;
    struct nk_rect bounds; nk_layout_peek(&bounds, ui_ctx); bounds.w -= 10; // bounds.w *= 0.95f;
    struct nk_vec2 item_padding = style->text.padding;
    struct nk_text text;
    text.padding.x = item_padding.x;
    text.padding.y = item_padding.y;
    text.background = style->window.background;
    text.text = nk_rgba_f(1,1,1,ui_alpha);

        nk_size val = *slider * 1000;
        int chg = nk_progress(ui_ctx, &val, 1000, NK_MODIFIABLE);
        *slider = val / 1000.f;

    chg |= input(MOUSE_L) && nk_input_is_mouse_hovering_rect(&ui_ctx->input, bounds); // , true);

    nk_widget_text(&win->buffer, bounds, caption, strlen(caption), &text, NK_TEXT_RIGHT, style->font);
    return chg;
}

int ui_short(const char *label, short *v) {
    if( label && ui_filter && ui_filter[0] ) if( !strstri(label, ui_filter) ) return 0;

    int i = *v, ret = ui_int( label, &i );
    return *v = (short)i, ret;
}


int ui_radio(const char *label, const char **items, int num_items, int *selector) {
    if( label && ui_filter && ui_filter[0] ) if( !strstri(label, ui_filter) ) return 0;

    int ret = 0;
    if( label && label[0] ) ui_label(label);
    for( int i = 0; i < num_items; i++ ) {
        bool enabled = *selector == i;
        if( ui_bool( va("%s%s", label && label[0] ? "  ":"", items[i]), &enabled ) ) {
            *selector = i;
            ret |= 1;
        }
    }
    return ret;
}


int ui_browse(const char **output, bool *inlined) {
    int clicked = 0;

#if HAS_IMAGE

    static struct browser_media media = {0};
    static struct browser browsers[2] = {0}; // 2 instances max: 0=inlined, 1=windowed
    static char *results[2] = {0}; // 2 instances max: 0=inlined, 1=windowed
    do_once {
        const int W = 96, H = 96; // 2048x481 px, 21x5 cells
        texture_t i = texture("icons/suru.png", TEXTURE_RGBA|TEXTURE_MIPMAPS);
        browser_config_dir(icon_load_rect(i.id, i.w, i.h, W, H, 16, 3), BROWSER_FOLDER); // default group
        browser_config_dir(icon_load_rect(i.id, i.w, i.h, W, H,  2, 4), BROWSER_HOME);
        browser_config_dir(icon_load_rect(i.id, i.w, i.h, W, H, 17, 3), BROWSER_COMPUTER);
        browser_config_dir(icon_load_rect(i.id, i.w, i.h, W, H,  1, 4), BROWSER_PROJECT);
        browser_config_dir(icon_load_rect(i.id, i.w, i.h, W, H,  0, 4), BROWSER_DESKTOP);

        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H,  8, 0), "");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H, 10, 2), ".txt.md.doc.license" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H,  8, 1), ".ini.cfg" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H,  8, 3), ".xlsx" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H,  9, 0), ".c" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H,  4, 1), ".h.hpp.hh.hxx" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H,  4, 2), ".fs.vs.gs.fx.glsl.shader" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H, 12, 0), ".cpp.cc.cxx" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H, 15, 0), ".json" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H, 15, 2), ".bat.sh" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H,  6, 1), ".htm.html" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H, 20, 1), ".xml" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H, 12, 1), ".js" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H,  0, 3), ".ts" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H,  6, 2), ".py" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H, 16, 1), ".lua" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H, 10, 0), ".css.doc" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H,  6, 0), ".wav.flac.ogg.mp1.mp3.mod.xm.s3m.it.sfxr.mid.fur" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H,  1, 3), ".ttf.ttc.otf" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H,  7, 1), ".jpg.jpeg.png.bmp.psd.pic.pnm.ico.ktx.pvr.dds.astc.basis.hdr.tga.gif" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H,  4, 3), ".mp4.mpg.ogv.mkv.wmv.avi" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H,  2, 1), ".iqm.iqe.gltf.gltf2.glb.fbx.obj.dae.blend.md3.md5.ms3d.smd.x.3ds.bvh.dxf.lwo" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H,  0, 1), ".exe" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H,  7, 0), ".bin.dSYM.pdb.o.lib.dll" ".");
        browser_config_type(icon_load_rect(i.id, i.w, i.h, W, H, 15, 3), ".zip.rar.7z.pak" ".");

        for( int j = 0; j < COUNTOF(browsers); ++j ) browser_init(&browsers[j]);
        browsers[0].listing = 1; // inlined version got listing by default, as there is not much space for layouting
    }
    // at_exit: browser_free(&browser);

    bool windowed = !inlined;
    if( windowed || (!windowed && *inlined) ) {

        struct browser *browser = browsers + windowed; // select instance
        char **result = results + windowed; // select instance

        struct nk_rect bounds = {0}; // // {0,0,400,300};
        bounds = nk_window_get_content_region(ui_ctx);
        if( !windowed && *inlined ) bounds.h *= 0.80;

        clicked = browser_run(ui_ctx, browser, windowed, bounds);
        if( clicked ) {
            strcatf(result, "%d", 0);
            (*result)[0] = '\0';
            strcatf(result, "%s", browser->file);
            if( inlined ) *inlined = 0;

            const char *target = ifdef(win32, "/", "\\"), *replace = ifdef(win32, "\\", "/");
            strswap(*result, target, replace);

            if( output ) *output = *result;
        }
    }
#endif

    return clicked;
}


/*
//  demo:
    static const char *file;
    if( ui_panel("inlined", 0)) {
        static bool show_browser = 0;
        if( ui_button("my button") ) { show_browser = true; }
        if( ui_browse(&file, &show_browser) ) puts(file);
        ui_panel_end();
    }
    if( ui_window("windowed", 0) ) {
        if( ui_browse(&file, NULL) ) puts(file);
        ui_window_end();
    }
*/

