// -----------------------------------------------------------------------------
// immediate ui framework
// - rlyeh, public domain
//
// @fixme: tooltips in toolbars
// @fixme: docking colors
// @fixme: panel inside window
// @fixme: nest ui_browser() within a window or within a panel
// @fixme: do malloc() ttf contents before passing them to imgui. also cfg->FontDataOwnedByAtlas = true
// @fixme: UI_LABEL color bar in ui_list()
// @fixme: return value of ui_image()/ui_tileset() not pixel perfect
//
// @todo: ui_gallery()
// @todo: subwindows

#if !CODE

enum UI_FLAGS { // for windows and panels
    UI_OPEN = 1,
    UI_MENUS = 2,
    UI_CENTER = 4,
};

API int ui_notify(const char *label, const char *body);
API int ui_window(const char *label, int *flags);
API int   ui_panel(const char *label, int flags); // can be nested within a window, or freestanding
API int     ui_collapse(const char *label, const char *id);
API int       ui_section(const char *label);
API int       ui_separator();
API int       ui_int(const char *label, int value[1]);
API int       ui_int2(const char *label, int value[2]);
API int       ui_int3(const char *label, int value[3]);
API int       ui_unsigned(const char *label, unsigned value[1]);
API int       ui_unsigned2(const char *label, unsigned value[2]);
API int       ui_unsigned3(const char *label, unsigned value[3]);
API int       ui_bool(const char *label, bool *value);
API int       ui_float(const char *label, float value[1]);
API int       ui_float2(const char *label, float value[2]);
API int       ui_float3(const char *label, float value[3]);
API int       ui_float4(const char *label, float value[4]);
API int       ui_floatabs(const char *label, float value[1]);
API int       ui_vec2i(const char *label, vec2i *v);
API int       ui_vec3i(const char *label, vec3i *v);
API int       ui_vec2(const char *label, vec2 *v);
API int       ui_vec3(const char *label, vec3 *v);
API int       ui_vec4(const char *label, vec4 *v);
API int       ui_mat33(const char *label, float M[9]);
API int       ui_mat34(const char *label, float M[12]);
API int       ui_mat44(const char *label, float M[16]);
API int       ui_buffer(const char *label, char *buffer, int buflen);
API int       ui_string(const char *label, char **string);
API int       ui_color3(const char *label, unsigned *color); //[0..255]
API int       ui_color3f(const char *label, float color[3]); //[0..1]
API int       ui_color4(const char *label, unsigned *color); //[0..255]
API int       ui_color4f(const char *label, float color[4]); //[0..1]
API int       ui_button(const char *label);
API int       ui_buttons(int buttons, /*labels*/...);
API int       ui_toolbar(const char *label, int icons, /*icon labels*/...); // returns 0 (no opt), or [1..N] option
API int       ui_toggle(const char *label, bool *value);
API int       ui_alert(const char *message, int *show);
API int       ui_dialog(const char *label, const char *text, int choices);
API int       ui_list(const char *label, const char **items, int num_items, int *selector);
API int       ui_bitmask(const char *label, int bits, void *ptr);
API int       ui_label(const char *label);
API int       ui_label2(const char *label, const char *caption);
API int       ui_slider(const char *label, float *value);
API int       ui_slider2(const char *label, float *value, const char *caption);
API int       ui_const_bool(const char *label, bool);
API int       ui_const_float(const char *label, float);
API int       ui_const_float2(const char *label, float[2]);
API int       ui_const_string(const char *label, const char *);
API int       ui_image(const char *label, unsigned id, unsigned w, unsigned h); // (-1,-1) to use (avail_width,line_height) size. returns XXYY percentages when clicked
API int       ui_subimage(const char *label, unsigned id, unsigned iw, unsigned ih, float u0, float v0, float u1, float v1); // returns XXYY percentages when clicked
API int       ui_clampi(const char *label, int *value, int mini, int maxi);
API int       ui_clampf(const char *label, float *value, float minf, float maxf);
API int       ui_percent(const char *label, float value); // use [0..1], [0..100], [0..1000] or NaN
API int       ui_link(const char *label, const char *url);
API int     ui_collapse_end();
API int   ui_panel_end();
API int   ui_tray(); // can be nested within a window (local tray), or freestanding (desktop tray)
API int     ui_menu(const char *label); // must be nested within tray
API int       ui_case(const char *label, bool disabled); // must be nested within menu
API int     ui_menu_end();
API int   ui_tray_end();
API int ui_window_end();

API int   ui_print(const char *title, const char *text); // printfs to a console window. \b clears log
API char* ui_console(const char *title); // displays console window. returns sent command or NULL if none

// demo
API int ui_demo(int do_windows);

// utils
API int ui_enable(bool);
API int ui_enabled();

API int ui_hide(bool);
API int ui_hidden();

API int ui_active();
API int ui_hovered();

#ifndef UI_ICON
#define UI_ICON(GLYPH) ICON_MS_##GLYPH // @fixme: ui_iconsloaded() depending on whether icon font was loaded
#endif

// internals
API int ui_loadfont(unsigned slot, float pt, float brightness, const char *resource, const void *runes);
API int ui_loadfonts();

 enum BROWSER_FLAGS {
    BROWSER_OPEN = 1,
    BROWSER_OPEN1 = 2,
    BROWSER_SAVE = 4,
    BROWSER_INLINE = 8,
};

API int    ui_browse(const char *winid, const char *wintitle, const char *pathmasks, unsigned flags);
API char** ui_browse_results(const char *winid, int *count);

// macros

#define ui_array(label,type,ptr) do { \
    int changed = 0; \
    if( ui_collapse(label, va(#type "%p",ptr)) ) { \
        char label_ex[8]; \
        for( int idx = 0, iend = array_count(*(ptr)); idx < iend; ++idx ) { \
            type* it = *(ptr) + idx; \
            snprintf(label_ex, sizeof(label_ex), "[%d]", idx); \
            changed |= ui_##type(label_ex, it); \
        } \
        ui_collapse_end(); \
    } \
} while(0)

#else
#include <float.h> // FLT_MIN,FLT_MAX

int igTextLineHeight() {
    return igGetFontSize() + igGetStyle()->FramePadding.y * 2; 
}
int igButton2(const char *text, ImVec2 align) {
    char *tooltip = strchr(text, '@');
    if( !tooltip ) return igButton(text, align);
    static char label[256];
    snprintf(label, 256, "%.*s", (int)(tooltip - text), text);
    int ret = igButton(label, align);
    if( igIsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) )
        igSetTooltip(tooltip+1);
    return ret;
}

int ui_hue = 0;
int ui_debug = 1;
uint64_t ui_frame = 0;
unsigned ui_hue32;

int ui_is_hidden = 0;

ImFont *ui_fonts[4] = {0};


int ui_browse(const char *winid, const char *wintitle, const char *pathmasks, unsigned flags) {
    if( flags & BROWSER_SAVE ) { // folder
        return igFileDialogSave(winid, wintitle, pathmasks);
    } else { // file(s)
        return (flags & BROWSER_OPEN1 ? igFileDialogOpen1 : igFileDialogOpen)(winid, wintitle, pathmasks);
    }
}
char** ui_browse_results(const char *winid, int *count) {
    return igFileDialogResults(winid, count);
}

const
int ui_slider_flags = 0;
// ImGuiSliderFlags_Logarithmic = 1 << 5,
// ImGuiSliderFlags_NoRoundToFormat = 1 << 6,
// ImGuiSliderFlags_NoInput = 1 << 7,
// ImGuiSliderFlags_WrapAround = 1 << 8,
// ImGuiSliderFlags_ClampOnInput = 1 << 9,
// ImGuiSliderFlags_ClampZeroRange = 1 << 10,
// ImGuiSliderFlags_AlwaysClamp = ImGuiSliderFlags_ClampOnInput | ImGuiSliderFlags_ClampZeroRange,

char *ui_filter;
void ui_filterbar()
{
    static int location = 0<<1|1<<0; // top-right
    ImGuiIO* io = igGetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings /*| ImGuiWindowFlags_NoFocusOnAppearing*/ | ImGuiWindowFlags_NoNav;
    ImGuiViewport* viewport = igGetMainViewport();
    if (location >= 0) {
        const float PAD = 10.0f;
        ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
        igSetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        igSetNextWindowViewport(viewport->ID);
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    // igSetNextWindowBgAlpha(0.35f); // Transparent background
    if (igBegin("##SearchFilter", NULL, window_flags)) {
        char *bak = ui_filter; ui_filter = 0;
        ui_string(UI_ICON(SEARCH) "Filter", &bak);
        ui_filter = bak;
    }
    igEnd();
}


int ui_global_id;
void ui_tick() {
    // new frame
    ui_frame++;

    // restart the global ID counter
    ui_global_id = 0;

    // reset hue
    ui_hue = 0;
    ImVec4 color; ui_hue32 = igGetColorU32_Vec4((ImColor_HSV((ImColor*)&color, ui_hue / 7.0f, 0.7f, 0.7f, 1), color));

    // this special panel comes first. it has top priority in panels stack
    if( ui_debug )
    ui_panel("Debug##Default",0) && ui_panel_end(); // this one comes first

    // display filter toolbar if requested
    static bool filter_open = 0;
    if( input_down(KEY_F) && input(KEY_CTRL) ) {
        filter_open ^= 1;
    }
    if( input_down(KEY_ESC) ) {
        filter_open = 0;
    }

    if( filter_open ) {
        ui_filterbar();
    } else {
        if( ui_filter ) FREE( ui_filter );
        ui_filter = 0;
    }
}





int ui_window(const char *label, int *flags) {
    bool wopen = !!(*flags & UI_OPEN);
    if( wopen ) {
        igPushID_Int(++ui_global_id);

        if(*flags & UI_OPEN/*UI_CENTER*/) {
        ImVec2 center = {0};
        ImGuiViewport_GetCenter(&center, igGetMainViewport());
        igSetNextWindowPos(center, ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
        }

        igSetNextWindowSize(ImVec2(200,1.5*200), ImGuiCond_FirstUseEver);
        igBegin(label, &wopen, /*ImGuiWindowFlags_NoCollapse|*/(*flags & UI_MENUS ? ImGuiWindowFlags_MenuBar:0)/*|ImGuiWindowFlags_AlwaysAutoResize*/);
        //igPushItemWidth(-100); // fill the space and leave 100 pixels for the label
        igPushItemWidth(-FLT_MIN); // remove the blank space typically used for the labels
        *flags = (*flags & ~UI_OPEN) | (wopen & UI_OPEN);
        return 1;
    }
    return 0;
}
int ui_window_end() {
    igEnd();
    igPopID();
    return 1;
}

static
void ui_adjust_nextpanel(const char *label, int flags) {
    static set(char*) set = 0; if(!set) set_init_str(set);
    char **found = set_find(set, (char*)label); int first = !found;
    if( first ) {
        set_insert(set, STRDUP(label));
        ImGuiViewport* viewport = igGetMainViewport();
        ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot = ImVec2(0,0);
        static int items = 0;
        const float PAD = !!work_pos.y * 5.0f;
//      printf("work_pos(%f,%f) work_size(%f,%f)\n", work_pos.x,work_pos.y, work_size.x,work_size.y);
        window_pos.x = (work_pos.x           + PAD);
        window_pos.y = ((work_pos.y+!!work_pos.y) * ++items + PAD);
        igSetNextWindowPos(window_pos, ImGuiCond_Once, window_pos_pivot);
        igSetNextWindowSize(ImVec2(work_size.x*0.33,work_size.y-window_pos.y), ImGuiCond_Once);
        igSetNextWindowViewport(viewport->ID);
        igSetNextWindowCollapsed(flags & UI_OPEN ? 0 : 1, ImGuiCond_Once);
    }

    if(flags & UI_CENTER) {
    ImVec2 center = {0};
    ImGuiViewport_GetCenter(&center, igGetMainViewport());
    igSetNextWindowPos(center, ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
    }
}

int ui_panel(const char *label, int flags) {// may be embedded inside a window, or standalone
    // postpone panel creation a few frames. to let main menubar be settled.
    if( ui_frame <= 2 ) return 0;

    igPushID_Int(++ui_global_id);

    ui_adjust_nextpanel(label, flags);

    igBegin(label, NULL, 0|ImGuiWindowFlags_NoDocking|(flags & UI_MENUS ? ImGuiWindowFlags_MenuBar:0));
    //igPushItemWidth(-100); // fill the space and leave 100 pixels for the label
    igPushItemWidth(-FLT_MIN); // remove the blank space typically used for the labels
    return 1;
}
int ui_panel_end() {
    igEnd();
    igPopID();
    return 1;
}

int ui_separator() {
    return ui_section(NULL);
}


int ui_notify(const char *label, const char *body) {
    int type = 'i'; // (i)nfo, o(k), (e)rror, (w)arning types
    /**/ if( strstri(label, "warn") ) type = 'w';
    else if( strstri(label, "err") || strstri(label,"fail") ) type = 'e';
    else if( strstri(label, "succ") || strstri(label,"complet") ) type = 'k';
    else if( strstri(label, "random") ) type = "weik"[rand()%4];
    return igInsertNotification(type, label, body, strchr("ki",type) ? 3 : -1), 1;
}


int ui_tray_global = 1;
int ui_tray() {
    ui_tray_global = igCurrentWindowStackSize() <= 1;
    return ui_tray_global ? igBeginMainMenuBar() : igBeginMenuBar();
}
int ui_tray_end() {
    ui_tray_global ? igEndMainMenuBar() : igEndMenuBar();

    return 1;
}
int ui_menu(const char *label) {
    bool enabled = 1;
    return igBeginMenu(label, enabled);
}
int ui_menu_end() {
    return igEndMenu(), 1;
}
int ui_case(const char *label, bool disabled) {
    const char *icon = NULL;
    const char *shortcut = NULL;
    bool enabled = !disabled;
    bool selected = false;
    return igMenuItemEx(label,icon,shortcut,selected,enabled);
}


/// [src] https://github.com/ocornut/imgui/issues/3565#issuecomment-2120416586 by azmr
/// Make contents in a table cover the entire row rather than just a single column.
///
/// Always covers the entire row (not just the remaining columns);
/// can sort of coexist with per-column data, but may not be as intended.
/// Accounts for:
/// - scrollbar
/// - header row
/// - column reordering
/// - hidden columns
float igTableFullRowBegin() {
    ImGuiTable *table = igGetCurrentTable();

    // Set to the first visible column, so that all contents starts from the leftmost point
    for (ImGuiTableColumnIdx *clmn_idx = table->DisplayOrderToIndex.Data,
         *end = table->DisplayOrderToIndex.DataEnd;
         clmn_idx < end; ++clmn_idx)
    {   if (igTableSetColumnIndex(*clmn_idx)) break;   }

    ImRect *work_rect    = &igGetCurrentWindow()->WorkRect;
    float   restore_x    = work_rect->Max.x;
    ImRect  bg_clip_rect = table->BgClipRect; // NOTE: this accounts for header column & scrollbar

    igPushClipRect(bg_clip_rect.Min, bg_clip_rect.Max, 0); // ensure that both our own drawing...
    work_rect->Max.x = bg_clip_rect.Max.x;                 // ...and Dear ImGui drawing will be visible across the entire row

    return restore_x;
}
void igTableFullRowEnd(float restore_x) {
    igGetCurrentWindow()->WorkRect.Max.x = restore_x;
    igPopClipRect();
}

static float UI_LABEL_RESTORE;
static int UI_LABEL_1OF2(const char *label) {
    enum { BOLD_INDENT = 10, BOLD_ALPHA = 150, BOLD_COLOR = ImGuiCol_Text };
    enum { BAR_MIN_WIDTH = 2, BAR_MAX_WIDTH = 4, BAR_HEIGHT = 2 };
    enum { TABLE_FLAGS = ImGuiTableFlags_Resizable }; // |ImGuiTableFlags_SizingStretchProp|ImGuiTableFlags_ContextMenuInBody|ImGuiTableFlags_NoHostExtendX };

    if( label && ui_filter && ui_filter[0] ) if( !strstri(label, ui_filter) ) return 0;

    const char *tooltip = 0;
    int has_label = label && label[0], hovered = 0;
    if(!igBeginTable("##", 2, TABLE_FLAGS, ImVec2(0,0), 0.f)) 
        return 0;

    igTableNextColumn();

    if(!has_label) label = "";

        static char copy[128];
        tooltip = strrchr(label, '@');
        if( tooltip ) snprintf(copy, 128, "%.*s %s", (int)(tooltip - label), label, UI_ICON(HELP) );

        igTextWithHoverColor(igGetColorU32_Col(BOLD_COLOR, BOLD_ALPHA / 100.f ), ImVec2(BOLD_INDENT/2,BOLD_INDENT), tooltip ? copy : label);
        hovered = igIsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled);

        ImVec2 rmin; igGetItemRectMin(&rmin); rmin.y -= BAR_HEIGHT;
        ImVec2 rmax; igGetItemRectMax(&rmax); rmax.y += BAR_HEIGHT;

        igRenderFrame(rmin,ImVec2(rmin.x+(hovered ? BAR_MAX_WIDTH : BAR_MIN_WIDTH),rmax.y),ui_hue32,false/*borders*/,0/*rounding*/);

        if( tooltip && hovered ) igSetTooltip("%s", tooltip + 1);

    igTableNextColumn();
    igPushItemWidth(-1);

    if(!has_label) UI_LABEL_RESTORE = igTableFullRowBegin();
    return 1;
}
static int UI_LABEL_2OF2( int rc ) {
    if( UI_LABEL_RESTORE ) igTableFullRowEnd(UI_LABEL_RESTORE), UI_LABEL_RESTORE = 0;
    return igPopItemWidth(), igEndTable(), rc;
}
#define UI_LABEL(label, ...) (UI_LABEL_1OF2(label) ? UI_LABEL_2OF2(__VA_ARGS__) : 0)


int ui_label(const char *label) {
    return UI_LABEL(label, (igText(""), 1));
}
int ui_label2(const char *label, const char *text) {
    return UI_LABEL(label, (igText("%s", text), 1));
}
struct InputTextCallback_UserData {
    char*                  Str;
};
static int InputTextCallback(ImGuiInputTextCallbackData* data) {
    struct InputTextCallback_UserData* user_data = (struct InputTextCallback_UserData*)data->UserData;
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        data->Buf = user_data->Str = REALLOC(user_data->Str, data->BufTextLen + 1); // @fixme: +1 needed?
    }
    return 0;
}
int ui_string(const char *label, char **str) {
    if(!UI_LABEL_1OF2(label)) return 0;

    strcatf(str, "", "");

    struct InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = *str;
    int rc = igInputText(label, *str, strlen(*str)+1, ImGuiInputTextFlags_CallbackCharFilter|ImGuiInputTextFlags_CallbackResize, InputTextCallback, &cb_user_data);
    *str = cb_user_data.Str;

    return UI_LABEL_2OF2(rc);
}
int ui_buffer(const char *label, char *buffer, int buflen) {
    return UI_LABEL(label, igInputText("##", buffer, buflen, 0/*ImGuiInputTextFlags*/, NULL, NULL));
}
int ui_const_string(const char *label, const char *text) {
    return ui_label2(label, text), 0;
}
int ui_const_bool(const char *label, bool value) {
    return ui_bool(label, &value), 0;
}
int ui_const_float(const char *label, float value) {
    return ui_float(label, &value), 0;
}
int ui_const_float2(const char *label, float value[2]) {
    float copy2[] = { value[0], value[1] };
    return ui_float2(label, copy2), 0;
}

int ui_clampi(const char *label, int *v, int mini, int maxi) {
    int range[] = { mini, maxi };
    return UI_LABEL(label, igDragScalarN("##", ImGuiDataType_S32,v,1,1.f,range+0,range+1,NULL,0));
}
int ui_clampf(const char *label, float *v, float minf, float maxf) {
    float range[] = { minf, maxf };
    return UI_LABEL(label, igDragScalarN("##", ImGuiDataType_Float,v,1,1.f,range+0,range+1,NULL,0));
}
int ui_int(const char *label, int v[1]) {
    // return UI_LABEL(label, igInputInt("##", v, 1, 10, 0));
    return UI_LABEL(label, igDragScalarN("##",ImGuiDataType_S32,v,1,1.f,NULL,NULL,NULL,0));
}
int ui_int2(const char *label, int v[2]) {
    return UI_LABEL(label, igDragScalarN("##",ImGuiDataType_S32,v,2,1.f,NULL,NULL,NULL,0));
}
int ui_int3(const char *label, int v[3]) {
    return UI_LABEL(label, igDragScalarN("##",ImGuiDataType_S32,v,3,1.f,NULL,NULL,NULL,0));
}
int ui_unsigned(const char *label, unsigned v[1]) {
    return UI_LABEL(label, igDragScalarN("##",ImGuiDataType_U32,v,1,1.f,NULL,NULL,NULL,0));
}
int ui_unsigned2(const char *label, unsigned v[2]) {
    return UI_LABEL(label, igDragScalarN("##",ImGuiDataType_U32,v,2,1.f,NULL,NULL,NULL,0));
}
int ui_unsigned3(const char *label, unsigned v[3]) {
    return UI_LABEL(label, igDragScalarN("##",ImGuiDataType_U32,v,3,1.f,NULL,NULL,NULL,0));
}

int ui_bool(const char *label, bool *value) {
    return UI_LABEL(label, igCheckbox("##", value));
}
int ui_toggle(const char *label, bool *value) {
    return UI_LABEL(label, igCheckbox("##", value));
}
int ui_float(const char *label, float *v) {
//    return UI_LABEL(label, igSliderFloat("##", v, 0.0f, 1.0f, "%.3f", ui_slider_flags));
//    return UI_LABEL(label, igDragFloat("##", v, 0.005f, 0.0f, 1.0f, "%.2f", 0/*ImGuiSliderFlags_AlwaysClamp*/));
//    float zero = 0, one = 1; return UI_LABEL(label, igSliderScalar("##", ImGuiDataType_Float, v, &zero, &one));
    return UI_LABEL(label, igDragFloat("##", v, 0.005f, 0, 0, "%.3f", 0 ));
}
int ui_floatabs(const char *label, float *v) {
    int rc = ui_float(label, v); if( *v < 0 ) *v = 0;
    return rc;
}
int ui_percent(const char *label, float pct) {
    if( pct != pct ) return UI_LABEL(label, (igProgressBar(-1.0f * igGetTime(),ImVec2(-1,0),NULL),0));
    if( pct > 100 ) return ui_percent(label, pct / 1000);
    if( pct > 1 ) return ui_percent(label, pct / 100);
    return UI_LABEL(label, (igProgressBar(pct,ImVec2(-1,0),NULL), 0));
}
int ui_float2(const char *label, float v[2]) {
    return UI_LABEL(label, igInputFloat2("##", v, "%f", ui_slider_flags));
}
int ui_float3(const char *label, float v[3]) {
    return UI_LABEL(label, igInputFloat3("##", v, "%f", ui_slider_flags));
}
int ui_float4(const char *label, float v[4]) {
    return UI_LABEL(label, igInputFloat4("##", v, "%f", ui_slider_flags));
}
int ui_slider(const char *label, float *value) {
    return UI_LABEL(label, igSliderFloat("##", value, 0, 1, "%f", ui_slider_flags));
}
int ui_slider2(const char *label, float *value, const char *caption) {
    return UI_LABEL(label, igSliderFloat("##", value, 0, 1, caption, ui_slider_flags));
}

int ui_mat33(const char *label, float M[9]) {
//    ui_num_signs = 3;
    int changed = 0;
    changed |= ui_label(label);
    changed |= ui_float3(NULL, M);
    changed |= ui_float3(NULL, M+3);
    changed |= ui_float3(NULL, M+6);
    return changed;
}
int ui_mat34(const char *label, float M[12]) {
//    ui_num_signs = 3;
    int changed = 0;
    changed |= ui_label(label);
    changed |= ui_float4(NULL, M);
    changed |= ui_float4(NULL, M+4);
    changed |= ui_float4(NULL, M+8);
    return changed;
}
int ui_mat44(const char *label, float M[16]) {
//    ui_num_signs = 4;
    int changed = 0;
    changed |= ui_label(label);
    changed |= ui_float4(NULL, M);
    changed |= ui_float4(NULL, M+4);
    changed |= ui_float4(NULL, M+8);
    changed |= ui_float4(NULL, M+12);
    return changed;
}


int ui_color3f(const char *label, float color[3]) { //[0..1]
    // |ImGuiColorEditFlags_PickerHueWheel
    return UI_LABEL(label, igColorEdit3("##", color, ImGuiColorEditFlags_Float));
}
int ui_color3(const char *label, unsigned *color32) { //[0..255]
    ImVec4 color; igColorConvertU32ToFloat4(&color,(ImU32)*color32);
    int touched = ui_color3f(label, &color.x);
    if( touched ) *color32 = (unsigned)igColorConvertFloat4ToU32(color);
    return touched;
}

int ui_color4f(const char *label, float color[4]) { //[0..1]
    // |ImGuiColorEditFlags_PickerHueWheel
    return UI_LABEL(label, igColorEdit4("##", color, ImGuiColorEditFlags_Float|ImGuiColorEditFlags_AlphaPreviewHalf|ImGuiColorEditFlags_AlphaBar));
}
int ui_color4(const char *label, unsigned *color32) { //[0..255]
    ImVec4 color; igColorConvertU32ToFloat4(&color,(ImU32)*color32);
    int touched = ui_color4f(label, &color.x);
    if( touched ) *color32 = (unsigned)igColorConvertFloat4ToU32(color);
    return touched;
}

int ui_button(const char *label) {
    igPushID_Int(++ui_global_id);
    int rc = igButton2(label, ImVec2(-1,0));
    return igPopID(), rc;
}
int ui_buttons(int buttons, /*labels*/...) {
    const char *labels[16] = {0};
    if( buttons >= COUNTOF(labels) ) die("ui_buttons() capacity exceeded");
        va_list list;
        va_start(list, buttons);
        for( int i = 0; i < buttons; ++i ) {
            labels[i] = va_arg(list, const char*);
        }
        va_end(list);

    int touched = 0;

    igPushID_Int(++ui_global_id);

    // special case
    if( buttons == 2 && !strcmp(labels[1], "...") ) {
        igSetNextItemAllowOverlap();
        if( igButton2(labels[0], ImVec2(-1,0)) ) touched = 1;
        igSameLine(0,0);
        igSetCursorPosX(igGetCursorPosX()-20);
        if( igSmallButton(labels[1]) ) touched = 2;
    }

    // general case
    else {
        igColumns(buttons, "##", false);
        for( int i = 0; i < buttons; ++i ) {
            if( igButton2(labels[i], ImVec2(-1,0)) )
                touched = i + 1;
            igNextColumn();
        }
    }

    igPopID();

    return touched;
}
int ui_toolbar(const char *label, int icons, ...) {
    if( !UI_LABEL_1OF2(label) )
        return 0;

    const char *labels[16] = {0};
    if( icons >= COUNTOF(labels) ) die("ui_toolbar() capacity exceeded");
        va_list list;
        va_start(list, icons);
        for( int i = 0; i < icons; ++i ) {
            labels[i] = va_arg(list, const char*);
        }
        va_end(list);

    igPushStyleVar_Float(ImGuiStyleVar_FrameRounding, 0);

    int icon = 0;
    for( int i = 0; i < icons ; ++i ) {
        if( /*igSmallButton*/igButton2(labels[i],ImVec2(0,0))) {
            icon = i+1;
        }
        if( i < (icons-1) ) igSameLine(0,0);
    }

    igPopStyleVar(1);

    return UI_LABEL_2OF2(icon);
}

// @fixme: show arg not needed?
int ui_dialog(const char *label, const char *text, int choices) {// @fixme: return
    int selected = 0;

    if (igButton2(label,ImVec2(0,0)))
        igOpenPopup_Str(label,0);

    if (igBeginPopupModal(label, NULL, 0/*ImGuiWindowFlags_MenuBar*/)) {
        igText("%s", text);

        if( choices > 0 ) {
            selected = ui_buttons(choices > 3 ? 3 : choices, "OK", "Cancel", "Retry");
            if( selected ) {
                igCloseCurrentPopup();
            }
        } else {
            if (igButton2("Close",ImVec2(0,0)))
                selected = 1, igCloseCurrentPopup();
        }

        igEndPopup();
    }
    return selected;
}
int ui_alert(const char *message, int *open) {
    if(*open == 1) igOpenPopup_Str("Alert", 0), *open = 2;

    if (igBeginPopupModal("Alert", NULL, 0/*ImGuiWindowFlags_MenuBar*/)) {
        igText("%s", message);
        int selected = ui_buttons(1, "OK");
        if( selected ) igCloseCurrentPopup(), *open = 0;
        igEndPopup();
    }
    return *open;
}
int ui_bitmask(const char *label, int numbits, void *ptr) {
    if( !UI_LABEL_1OF2(label) )
        return 0;

    uintptr_t addr = 0;
    memcpy(&addr, ptr, numbits/8);

    int touched = 0;
    for( int i = 0; i < numbits ; ++i ) {
        if( i > 0 ) igSameLine(0,0);
        igPushID_Int(i);
        if( igSmallButton(addr & (1ull<<i) ? "1" : "0") ) {
            touched = i+1;
            addr ^= (1ull<<i);
            memcpy(ptr, &addr, numbits/8);
        }
        igPopID();
    }

    return UI_LABEL_2OF2(touched);
}

int ui_list(const char *label, const char **items, int num_items, int *selector) {
    int height_in_items = num_items > 4 ? 4 : num_items;
    return UI_LABEL(label, igListBox_Str_arr("##",selector,items,num_items,height_in_items));
}

int ui_section(const char *label) {
    ui_hue = (ui_hue + 1) % 7;

    if( label && ui_filter && ui_filter[0] ) if( !strstri(label, ui_filter) ) return 0;

    ImVec4 color; ui_hue32 = igGetColorU32_Vec4((ImColor_HSV((ImColor*)&color, ui_hue / 7.0f, 0.7f, 0.7f, 1), color));

        if(ui_fonts[1]) igPushFont(ui_fonts[1]);

#if 0 // colorful
        igPushStyleColor_U32(ImGuiCol_Separator, ui_hue32);
        int ret = UI_LABEL(NULL, (igSeparatorText(label ? label : ""), 0));
        igPopStyleColor(1);
#else
        int ret = (igSeparatorText(label ? label : ""), 0);
#endif

        if(ui_fonts[1]) igPopFont();

    return ret;
}

int ui_subimage(const char *label, unsigned id, unsigned w, unsigned h, float u0, float v0, float u1, float v1) {
    if( !UI_LABEL_1OF2(label) )
        return 0;

    ImVec2 area;
    igGetContentRegionAvail(&area);
    if( w == ~0u ) w = area.x; // avail width
    if( h == ~0u ) h = igTextLineHeight(); // line height

    int ret = igImageButtonEx(++ui_global_id, id,
        ImVec2(w,h), ImVec2(u0,v0), ImVec2(u1,v1), ImVec4(0,0,0,0), ImVec4(1,1,1,1), 0);

    if( ret ) {
        ImVec2 click, src, dst;
        igGetMousePos(&click);
        igGetItemRectMin(&src);
        int px = 0x100 * (click.x - src.x) / area.x;
        int py = 0x100 * (click.y - src.y) / area.y;
        ret = px * 0x100 + py; // printf("%d %d xy:%d\n", px, py, ret);
    }

    return UI_LABEL_2OF2(ret);
}

int ui_image(const char *label, unsigned id, unsigned w, unsigned h) {
    return ui_subimage(label, id, w,h, 0,0,1,1);
}

int ui_is_enabled = 1;
int ui_enable(bool enable) {
    if( enable ^ ui_is_enabled ) {
        if( enable ) igEndDisabled(); else igBeginDisabled(1);
    }
    ui_is_enabled = enable;
    return 1;
}
int ui_enabled() {
    return ui_is_enabled;
}


int ui_hide(bool hidden) {
    ui_is_hidden = hidden;
    return 1;
}
int ui_hidden() {
    return ui_is_hidden;
}

int ui_active() {
    return igIsAnyItemActive();
}
int ui_hovered() {
    return igIsAnyItemHovered();
}

char *ui_console(const char *title) {
    extern char *igConsole(const char *);
    ui_adjust_nextpanel(title, 0);
    return igConsole(title);
}
int ui_print(const char *title, const char *text) {
    extern int igConsolePrintf(const char *title, const char *fmt, ...);
    return igConsolePrintf(title, "%s", text);
}


int ui_demo(int do_windows) {
    static int integer = 42;
    static bool toggle = true;
    static bool boolean = true;
    static float floating = 3.14159;
    static float float2[2] = {1,2};
    static float float3[3] = {1,2,3};
    static float float4[4] = {1,2,3,4};
    static float rgbf[3] = {0.84,0.67,0.17};
    static float rgbaf[4] = {0.67,0.90,0.12,0.5};
    static unsigned rgb = 0x22446688;
    static unsigned rgba = 0x88664422;
    static float slider = 0.5f;
    static float slider2 = 0.5f;
    static char buffer[64] = "hello world 123";
    static char *string = 0;
    static int item = 0; const char *list[] = {"one","two","three"};
    static bool show_dialog = false;
    static uint8_t bitmask = 0x55;
    static int hits = 0;
    static int window1 = 0, window2 = 0, window3 = 0;
    static int disable_all = 0;

    if( ui_panel("UI", 0) ) {
        int choice = ui_toolbar(NULL, 2,"Toast@Notify","Toggle on/off");
            if(choice == 1) ui_notify(va("My random toast (%d)", rand()), va("This is notification #%d", ++hits));
            if(choice == 2) disable_all ^= 1;
        
        if( disable_all ) ui_enable(0);

        if( ui_section("Labels")) {}
        if( ui_label("my label")) {}
        if( ui_label("my label with tooltip@built on " __DATE__ " " __TIME__)) {}
        if( ui_link("my link", "https://google.com"));
        if( ui_toolbar("my toolbar", 4, UI_ICON(STAR),UI_ICON(STAR_HALF),UI_ICON(BOOKMARK),UI_ICON(BOOKMARK_STAR)) ) {}
        //if( ui_label2_wrap("my long label", "and some long long long long text wrapped")) {}

        if( ui_section("Types")) {}
        if( ui_bool("my bool", &boolean) ) puts("bool changed");
        if( ui_int("my int", &integer) ) puts("int changed");
        if( ui_float("my float", &floating) ) puts("float changed");
        if( ui_buffer("my buffer", buffer, 64) ) puts("buffer changed");
        if( ui_string("my string", &string) ) puts("string changed");

        if( ui_section("Vectors") ) {}
        if( ui_float2("my float2", float2) ) puts("float2 changed");
        if( ui_float3("my float3", float3) ) puts("float3 changed");
        if( ui_float4("my float4", float4) ) puts("float4 changed");

        if( ui_section("Lists")) {}
        if( ui_list("my list", list, 3, &item ) ) puts("list changed");

        if( ui_section("Colors")) {}
        if( ui_color3("my color3", &rgb) ) puts("color3 changed");
        if( ui_color4("my color4@this is a tooltip", &rgba) ) puts("color4 changed");
        if( ui_color3f("my color3f", rgbf) ) puts("color3f changed");
        if( ui_color4f("my color4f@this is a tooltip", rgbaf) ) puts("color4f changed");

        if( ui_section("Sliders")) {}
        if( ui_slider("my slider", &slider)) puts("slider changed");
        if( ui_slider2("my slider 2", &slider2, va("%.2f", slider2))) puts("slider2 changed");
        if( ui_percent("my bar %", slider2));
        if( ui_percent("my bar INF", NAN));

        if( do_windows ) {
        if( ui_section("Windows")) {}
        int show = ui_buttons(3, "Container", "SubPanel", "SubRender");
        if( show == 1 ) window1 = 1;
        if( show == 2 ) window2 = 1;
        if( show == 3 ) window3 = 1;
        }

        if( ui_section("Others")) {}
        if( ui_bitmask("my bitmask", 8, &bitmask) ) printf("bitmask changed %x\n", bitmask);
        if( ui_toggle("my toggle", &toggle) ) printf("toggle %s\n", toggle ? "on":"off");

        int xy;
        if( (xy = ui_image("my image", 1/*image id #1*/, -1, -1)) ) { printf("image clicked at %04x\n", xy); }

        if( ui_separator() ) {}
        if( ui_button("my button") ) { puts("button clicked"); show_dialog = true; }
        if( ui_buttons(2, "yes", "no") ) { puts("button clicked"); }
        if( ui_buttons(3, "yes", "no", "maybe") ) { puts("button clicked"); }

        static int show_alert = 0;
        ui_alert("Button was pressed", &show_alert);

        if( ui_dialog("my dialog", __FILE__ "\n" __DATE__ "\n" "Public Domain.", 2/*two buttons*/) ) {
            show_alert = 1;
        }

        if( disable_all ) ui_enable(1); // restore enabled state

        ui_panel_end();
    }

    // write log to a new console window
    if(rand() < RAND_MAX/100) ui_print("My console", va("This is a rand line #%d", rand()));

    // browser results (if any)
    int count;
    for( char **list = 0; (list = ui_browse_results("my browser", &count)); list = 0 ) 
        while(count--) puts(list[count]);

    // display console window, retrieve optional command sent to that window
    const char *cmd = ui_console("My console");
    if(cmd) puts(cmd);

    if( ui_button("Open file") ) ui_browse("my browser", "open file", "*.*", BROWSER_OPEN1);


    if( !do_windows ) return 0;


    // window api showcasing
    if( ui_window("Container demo", &window1) ) {
        ui_label("label #1");
        if( ui_bool("my bool", &boolean) ) puts("bool changed");
        if( ui_int("my int", &integer) ) puts("int changed");
        if( ui_float("my float", &floating) ) puts("float changed");
        if( ui_buffer("my string", string, 64) ) puts("string changed");
        ui_window_end();
    }

    if( ui_window("SubPanel demo", &window2) ) {
        if( ui_panel("panel #2", 0) ) {
            ui_label("label #2");
            if( ui_bool("my bool", &boolean) ) puts("bool changed");
            if( ui_int("my int", &integer) ) puts("int changed");
            if( ui_float("my float", &floating) ) puts("float changed");
            if( ui_buffer("my string", string, 64) ) puts("string changed");
            ui_panel_end();
        }
        ui_window_end();
    }

    if( ui_window("SubRender demo", &window3) ) {
        if( ui_panel("panel #3A", 0) ) {
            if( ui_bool("my bool", &boolean) ) puts("bool changed");
            if( ui_int("my int", &integer) ) puts("int changed");
            if( ui_float("my float", &floating) ) puts("float changed");
            if( ui_buffer("my string", string, 64) ) puts("string changed");
            if( ui_separator() ) {}
            if( ui_slider("my slider", &slider)) puts("slider changed");
            if( ui_slider2("my slider 2", &slider2, va("%.2f", slider2))) puts("slider2 changed");
            ui_panel_end();
        }
        if( ui_panel("panel #3B", 0) ) {
            if( ui_bool("my bool", &boolean) ) puts("bool changed");
            if( ui_int("my int", &integer) ) puts("int changed");
            if( ui_float("my float", &floating) ) puts("float changed");
            if( ui_buffer("my string", string, 64) ) puts("string changed");
            if( ui_separator() ) {}
            if( ui_slider("my slider", &slider)) puts("slider changed");
            if( ui_slider2("my slider 2", &slider2, va("%.2f", slider2))) puts("slider2 changed");
            ui_panel_end();
        }

#if HAS_IMAGE
        const char *title = "SubRender demo";
        struct nk_window *win = nk_window_find(ui_ctx, title);
        if( win ) {
            enum { menubar_height = 65 }; // title bar (~32) + menu bounds (~25)
            struct nk_rect bounds = win->bounds; bounds.y += menubar_height; bounds.h -= menubar_height;
#if 1
            ddraw_flush();

            // @fixme: this is breaking rendering when post-fxs are in use. edit: cannot reproduce
            static texture_t tx = {0};
            if( texture_rec_begin(&tx, bounds.w, bounds.h )) {
                glClearColor(0.15,0.15,0.15,1);
                glClear(GL_COLOR_BUFFER_BIT);
                ddraw_grid(10);
                ddraw_flush();
                texture_rec_end(&tx);
            }
            struct nk_image image = nk_image_id((int)tx.id);
            nk_draw_image_flipped(nk_window_get_canvas(ui_ctx), bounds, &image, nk_white);
#else
            static video_t *v = NULL;
            do_once v = video( "bjork-all-is-full-of-love.mp4", VIDEO_RGB );

            texture_t *textures = video_decode( v );

            struct nk_image image = nk_image_id((int)textures[0].id);
            nk_draw_image(nk_window_get_canvas(ui_ctx), bounds, &image, nk_white);
#endif
        }
#endif

        ui_window_end();
    }
    return 0;
}

#endif

#if 0
// How to do a toolbar in Dear ImGui.
// [src] https://gist.github.com/rmitton/f80cbb028fca4495ab1859a155db4cd8

enum { toolbarSize = 50 }; // cant be probably smaller than padding amounts added up together
float menuBarHeight;

void DockSpaceUI() {
    ImGuiViewport* viewport = igGetMainViewport();
    igSetNextWindowPos(ImVec2(viewport->Pos.x,viewport->Pos.y + toolbarSize), 0, ImVec2(0,0));
    igSetNextWindowSize(ImVec2(viewport->Size.x,viewport->Size.y - toolbarSize), 0);
    igSetNextWindowViewport(viewport->ID);
    ImGuiWindowFlags window_flags = 0
        | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
        | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    igPushStyleVar_Float(ImGuiStyleVar_WindowRounding, 0.0f);
    igPushStyleVar_Float(ImGuiStyleVar_WindowBorderSize, 0.0f);
    igBegin("Master DockSpace", NULL, window_flags);
    ImGuiID dockMain = igGetID_Str("MyDockspace");
    
    // Save off menu bar height for later.
    menuBarHeight = igGetCurrentWindow()->MenuBarHeight;

    igDockSpace(dockMain, ImVec2(0,0), 0, NULL);
    igEnd();
    igPopStyleVar(3);
}

void ToolbarUI() {
    ImGuiViewport* viewport = igGetMainViewport();
    igSetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + menuBarHeight), 0, ImVec2(0,0));
    igSetNextWindowSize(ImVec2(viewport->Size.x, toolbarSize), 0);
    igSetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = 0
        | ImGuiWindowFlags_NoDocking 
        | ImGuiWindowFlags_NoTitleBar 
        | ImGuiWindowFlags_NoResize 
        | ImGuiWindowFlags_NoMove 
        | ImGuiWindowFlags_NoScrollbar 
        | ImGuiWindowFlags_NoSavedSettings
        ;
    igPushStyleVar_Float(ImGuiStyleVar_WindowBorderSize, 0);
    igBegin("TOOLBAR", NULL, window_flags);
  
    igButton2("Toolbar goes here", ImVec2(0,37));
  
    igEnd();

    igPopStyleVar(1);
}

void ProgramUI() {
    DockSpaceUI();
    ToolbarUI();
}
#endif

#if 0

API int   ui_collapse(const char *label, const char *id);
API int   ui_collapseo(const char *label, const char *id);
API int   ui_contextual();
API int    ui_short(const char *label, short *value);
API int    ui_double(const char *label, double *value);
API int   ui_contextual_end(int close);
API int   ui_collapse_clicked();
API int   ui_collapse_end();

// @todo: move to editor
API int  ui_display(const char *panel_or_window_title, int enabled);
API int  ui_setsize(const char *panel_or_window_title, float width, float height);
API int  ui_visible(const char *panel_or_window_title); // @todo: include ui_collapse() items that are open as well?
API int ui_popups(); // returns number of active popups

#endif

#if CODE

int ui_loadfont(unsigned slot, float pt, float brightness, const char *resource, const void *runes) {
    ImFontConfig*cfg = ImFontConfig_ImFontConfig();

    bool ok = 0;
    if( slot < countof(ui_fonts) && pt && brightness && resource ) {
        cfg->FontDataOwnedByAtlas = false; // TTF/OTF data ownership taken by the container ImFontAtlas (will delete memory itself).
        cfg->RasterizerMultiply = brightness;
        int len; char *bin = file_read(resource, &len);
        ok = !!(ui_fonts[slot] = ImFontAtlas_AddFontFromMemoryTTF(igGetIO()->Fonts, bin, len, pt, cfg, (const ImWchar *)runes));
    } else {
        ok = !!(ui_fonts[slot] = ImFontAtlas_AddFontDefault(igGetIO()->Fonts, cfg));
    }

    ImFontConfig_destroy(cfg);
    return ok;
}

int ui_loadfonts() { // return num loaded slots

#if 1
    #define UI_FONT0 15.f, 1.00f, "B612Regular.ttf"
    #define UI_FONT1 16.f, 1.50f, "B612BoldItalic.ttf"
#endif

    // load primary font [0]
    // else load os fallbacks for primary font [0]
    // else load embedded default font
    if( !ui_loadfont(0, UI_FONT0, NULL) )
#if !is(osx)
        if( !ui_loadfont(0, 15.f, 1.f, ifdef(win32,"C:/Windows/Fonts/Arial.ttf","/usr/share/fonts/liberation/LiberationSans-Regular.ttf"),NULL) )
#endif
            if( !ui_loadfont(0, 0, 0, 0, NULL)) ;
    
    // merge icon glyphs in primary font
    int len; char *bin = file_read("MaterialSymbolsSharp_Filled_28pt_Regular.ttf", &len);
    if( bin && len ) {
        static const unsigned/*ImWchar*/ runes[] = { ICON_MIN_MS, ICON_MAX_MS, 0, };

        ImFontConfig*cfg = ImFontConfig_ImFontConfig();
            cfg->FontDataOwnedByAtlas = false; // TTF/OTF data ownership taken by the container ImFontAtlas (will delete memory itself).
            cfg->OversampleH = 2; // Rasterize at higher quality for sub-pixel positioning. Note the difference between 2 and 3 is minimal. You can reduce this to 1 for large glyphs save memory. Read https://github.com/nothings/stb/blob/master/tests/oversample/README.md for details.
            cfg->OversampleV = 1; // Rasterize at higher quality for sub-pixel positioning. This is not really useful as we don't use sub-pixel positions on the Y axis.
            cfg->PixelSnapH = false;    // Align every glyph to pixel boundary. Useful e.g. if you are merging a non-pixel aligned font with the default font. If enabled, you can set OversampleH/V to 1.
            // ImVec2          GlyphExtraSpacing;      // 0, 0     // Extra spacing (in pixels) between glyphs. Only X axis is supported for now.
            cfg->GlyphOffset.y = 5;  // MD:8           // 0, 0     // Offset all glyphs from this font input.
            // const ImWchar*  GlyphRanges;            // NULL     // THE ARRAY DATA NEEDS TO PERSIST AS LONG AS THE FONT IS ALIVE. Pointer to a user-provided list of Unicode range (2 value per range, values are inclusive, zero-terminated list).
            // float           GlyphMinAdvanceX;       // 0        // Minimum AdvanceX for glyphs, set Min to align font icons, set both Min/Max to enforce mono-space font
            // float           GlyphMaxAdvanceX;       // FLT_MAX  // Maximum AdvanceX for glyphs
            cfg->MergeMode = true; // Merge into previous ImFont, so you can combine multiple inputs font into one ImFont (e.g. ASCII font + icons + Japanese glyphs). You may want to use GlyphOffset.y when merge font of different heights.
            // unsigned int    FontBuilderFlags;       // 0        // Settings for custom font builder. THIS IS BUILDER IMPLEMENTATION DEPENDENT. Leave as zero if unsure.
            // float           RasterizerMultiply;     // 1.0f     // Linearly brighten (>1.0f) or darken (<1.0f) font output. Brightening small fonts may be a good workaround to make them more readable. This is a silly thing we may remove in the future.
            // float           RasterizerDensity;      // 1.0f     // DPI scale for rasterization, not altering other font metrics: make it easy to swap between e.g. a 100% and a 400% fonts for a zooming display. IMPORTANT: If you increase this it is expected that you increase font scale accordingly, otherwise quality may look lowered.
            // ImWchar         EllipsisChar;           // -1       // Explicitly specify unicode codepoint of ellipsis character. When fonts are being merged first specified ellipsis will be used.
            ImFontAtlas_AddFontFromMemoryTTF(igGetIO()->Fonts, bin, len, 24.f, cfg, (const ImWchar*)runes);
        ImFontConfig_destroy(cfg);
    }

    // load bold font [1]
    // else load embedded default font
    if( !ui_loadfont(1, UI_FONT1, NULL) )
        ui_fonts[1] = ui_fonts[0];

    return !!ui_fonts[0] + !!ui_fonts[1] + !!ui_fonts[2] + !!ui_fonts[3];
}

int ui_vec2i(const char *label, vec2i *v) { return ui_unsigned2(label, (unsigned*)v); }
int ui_vec3i(const char *label, vec3i *v) { return ui_unsigned3(label, (unsigned*)v); }
int ui_vec2(const char *label, vec2 *v) { return ui_float2(label, (float*)v); }
int ui_vec3(const char *label, vec3 *v) { return ui_float3(label, (float*)v); }
int ui_vec4(const char *label, vec4 *v) { return ui_float4(label, (float*)v); }

int ui_link(const char *label, const char *url) {
    return UI_LABEL(label, (igTextLinkOpenURL(url, url), 0));
}

int ui_collapse(const char *label, const char *id) {
    if(!UI_LABEL_1OF2(NULL))
        return 0;

    igPushID_Str(id);
    igIndent(12);
    int ret = igCollapsingHeader_TreeNodeFlags(label,0);
    igIndent(-12);
    if(ret) igIndent(4);
    igPopID();

    return UI_LABEL_2OF2(ret);
}
int ui_collapse_end() {
    return igUnindent(4), 0;
}
//#define ui_collapse_clicked() 0



int execU(const char *cmd, const char *val) {
    if( !strcmp(cmd, "ui.debug") ) return ui_debug = atoi(val), 1;
    return 0;
}

AUTORUN {
    events("exec", execU);
}

#endif
