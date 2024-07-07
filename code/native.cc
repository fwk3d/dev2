#define API __declspec(dllexport)
#include "3rd/3rd_icon_ms.h"
#define UI_ICON(GLYPH) ICON_MS_##GLYPH

#include <SDL3/SDL.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_USE_WCHAR32

#include "3rd/cimgui/imgui/imgui_impl_opengl3.cpp"
#include "3rd/cimgui/imgui/imgui_impl_sdl3.cpp"

#include "3rd/cimgui/cimgui.cpp"
#include "3rd/cimgui/imgui/imgui_impl_sdl3.h"
#include "3rd/cimgui/imgui/imgui_impl_opengl3.h"

#include <SDL3/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#include "3rd/cimgui/imgui/imgui.cpp"
#include "3rd/cimgui/imgui/imgui_demo.cpp"
#include "3rd/cimgui/imgui/imgui_draw.cpp"
#include "3rd/cimgui/imgui/imgui_tables.cpp"
#include "3rd/cimgui/imgui/imgui_widgets.cpp"

//#include "3rd/edit/3rd_imLegitProfilerTask.h"
//#include "3rd/edit/3rd_imLegitProfiler.h"
#include "3rd/edit/3rd_imNodeFlow.hh"
#include "3rd/edit/3rd_imKnobs.hh"
#include "3rd/edit/3rd_imNotify.hh"
#include "3rd/edit/3rd_imPlot.hh"
#include "3rd/edit/3rd_imSequencer.hh"
#include "3rd/edit/3rd_imGuizmo.hh"
#include "3rd/3rd_stb_image.h"
#include "3rd/edit/3rd_imFileDialog.hh"

#include "3rd/cimplot/cimplot.cpp"
#pragma comment(lib, "opengl32") // needed?

#if (RLYEH_MOD1+RLYEH_MOD2+RLYEH_MOD3) != 3
#error DearImgui integrator, did you forget to include any of our custom modifications?
#endif

#include "engine.h"
#include "sys/sys_dialog.hh"
#include "game/game_ui_console.hh"

#include <stdio.h>
#include <stdlib.h>

// This example doesn't compile with Emscripten yet! Awaiting SDL3 support.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif



extern "C" {

/*static*/ SDL_Window* window;
/*static*/ SDL_Renderer* renderer;
/*static*/ SDL_GLContext glContext;
/*static*/ int app_keep = 1;
int app_msaa = 0;
float monitor_fps = 0;
extern uint64_t ui_frame;

volatile float target_fps;
}

static
void CherryTheme() {
    ImGui::StyleColorsDark();    // Reset styles

    // cherry colors, 3 intensities
    #define HI(v)   ImVec4(0.502f, 0.075f, 0.256f, v)
    #define MED(v)  ImVec4(0.455f, 0.198f, 0.301f, v)
    #define LOW(v)  ImVec4(0.232f, 0.201f, 0.271f, v)
    // backgrounds (@todo: complete with BG_MED, BG_LOW)
    #define BG(v)   ImVec4(0.200f, 0.220f, 0.270f, v)
    // text
    #define TXT(v) ImVec4(0.860f, 0.930f, 0.890f, v)

    auto &style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text]                  = TXT(0.78f);
    style.Colors[ImGuiCol_TextDisabled]          = TXT(0.28f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_ChildBg]               = BG( 0.58f);
    style.Colors[ImGuiCol_PopupBg]               = BG( 0.9f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]               = BG( 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]        = MED( 0.78f);
    style.Colors[ImGuiCol_FrameBgActive]         = MED( 1.00f);
    style.Colors[ImGuiCol_TitleBg]               = LOW( 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]         = HI( 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = BG( 0.75f);
    style.Colors[ImGuiCol_MenuBarBg]             = BG( 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg]           = BG( 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = MED( 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = MED( 1.00f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered]         = MED( 0.86f);
    style.Colors[ImGuiCol_ButtonActive]          = MED( 1.00f);
    style.Colors[ImGuiCol_Header]                = MED( 0.76f);
    style.Colors[ImGuiCol_HeaderHovered]         = MED( 0.86f);
    style.Colors[ImGuiCol_HeaderActive]          = HI( 1.00f);
//  style.Colors[ImGuiCol_Column]                = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
//  style.Colors[ImGuiCol_ColumnHovered]         = MED( 0.78f);
//  style.Colors[ImGuiCol_ColumnActive]          = MED( 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = MED( 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive]      = MED( 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = TXT(0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = MED( 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = TXT(0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = MED( 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = MED( 0.43f);
    // [...]
    style.Colors[ImGuiCol_ModalWindowDimBg]      = BG( 0.73f);
#if 0
    style.Colors[ImGuiCol_Tab] = MED(0.43f);
    style.Colors[ImGuiCol_TabHovered] = MED(0.73f);
    style.Colors[ImGuiCol_TabUnfocused] = MED(0.23f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = HI(0.23f);
    style.Colors[ImGuiCol_TabActive] = HI(0.43f);
#endif

    style.FrameRounding            = 3.0f;
    style.GrabRounding             = 2.0f;
    style.GrabMinSize              = 20.0f;
    style.ScrollbarSize            = 12.0f;
    style.ScrollbarRounding        = 16.0f;

    style.ItemSpacing.x            = 4;
    style.FramePadding.y = 0;
    style.ItemSpacing.y = 2;
#if 0
    style.WindowPadding            = ImVec2(6, 4);
    style.WindowRounding           = 0.0f;
    style.FramePadding             = ImVec2(5, 2);
    style.ItemSpacing              = ImVec2(7, 1);
    style.ItemInnerSpacing         = ImVec2(1, 1);
    style.TouchExtraPadding        = ImVec2(0, 0);
    style.IndentSpacing            = 6.0f;
#endif

    style.WindowTitleAlign.x = 0.50f;
    style.SelectableTextAlign.x = 0.50f;
    style.SeparatorTextAlign.x = 0.04f;
    style.SeparatorTextBorderSize = 1;
    style.SeparatorTextPadding = ImVec2(0,0);

    style.Colors[ImGuiCol_Header].w = 0/255.f; // collapsable headers
    style.Colors[ImGuiCol_TableBorderLight].w = 80/255.f; // column resizing grips

    style.Colors[ImGuiCol_Border] = ImVec4(0.539f, 0.479f, 0.255f, 0.162f);
    style.FrameBorderSize = 0.0f;
    style.WindowBorderSize = 1.0f;

    #undef HI
    #undef MED
    #undef LOW
    #undef BG
    #undef TXT
}

extern "C"
int sdl3_init(float scale, unsigned flags) {

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#endif

if(optioni("--gldebug",0))
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;

// MSAA
SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, app_msaa = (
    flags & APP_MSAA8 ? 8 :
    flags & APP_MSAA4 ? 4 :
    flags & APP_MSAA2 ? 2 : 0 )
);

// Position & Square
if(flags & APP_TRANSPARENT) window_flags |= SDL_WINDOW_TRANSPARENT;
int numdp; SDL_DisplayID *dps = SDL_GetDisplays(&numdp);
int monitor = dps[0];
if(scale <= 1) scale *= 100;
bool fullscreen = scale >= 100;
if(scale >= 100) scale = 100;
scale /= 100;
SDL_Rect bounds;
if( !SDL_GetDisplayUsableBounds(monitor, &bounds) ) die( SDL_GetError() );
int ww = bounds.w * scale, hh = bounds.h * scale;
int orientation = SDL_GetCurrentDisplayOrientation(monitor);
if( orientation == SDL_ORIENTATION_PORTRAIT || orientation == SDL_ORIENTATION_PORTRAIT_FLIPPED ) {
    int swap = ww; ww = hh; hh = swap;
}
if( flags & APP_SQUARE ) ww = hh = min(ww,hh);

    window = SDL_CreateWindow("", ww, hh, window_flags);
    if (!window) die(SDL_GetError());
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    glContext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glContext);

monitor_fps = SDL_GetCurrentDisplayMode(monitor)->refresh_rate;
float asked = optioni("--fps", monitor_fps); // 0 for immediate updates, 1 for updates synchronized with the vertical retrace, -1 for adaptive vsync, N to any given fps number. defaults to monitor Hz
/**/ if( asked ==  0 ) target_fps = 0, SDL_GL_SetSwapInterval( 0); // disable our fps limiter + vsync off: max speed
else if( asked ==  1 ) target_fps = 0, SDL_GL_SetSwapInterval( 1); // disable our fps limiter + vsync on : vsync caps fps to monitor hz
else if( asked  <  0 ) target_fps = 0, SDL_GL_SetSwapInterval(-1); // disable our fps limiter + adaptive : vsync when above monitor hz and off when it's below
else                   target_fps = asked + 0.5; // target specific framerate
//if( target_fps > 30 )  target_fps -= 3; // see: https://blurbusters.com/gsync/gsync101-input-lag-tests-and-settings/3/
//    SDL_ShowWindow(window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    auto *ctx = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

    if(flag("--viewports"))
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

#if 1
    io.IniFilename = ".settings.ini";
    CherryTheme();
#endif

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

if(optioni("--gldebug",0))
    glDebugEnable();

PRINTF("Build version: %d.%d.%d\n", BUILD_VERSION / 10000, (BUILD_VERSION / 100) % 100, BUILD_VERSION % 100);
PRINTF("Monitor: %s (vsync=%5.2fHz, requested=%f)\n", SDL_GetDisplayName(monitor), monitor_fps, asked);
PRINTF("GPU device: %s\n", glGetString(GL_RENDERER));
PRINTF("GPU driver: %s\n", glGetString(GL_VERSION));

    ui_loadfonts();

#if 1
    // imfiledialog
    igFileDialogInit();

    // imPlot
    do_once ImPlot::CreateContext();
    do_once ImPlot::SetImGuiContext(ctx);
#endif

    return 0;
}

extern "C"
void sdl3_tick() {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT)
            app_keep = 0;
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
            app_keep = 0;
    }

    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
    {
        if( flag("--throttle") ) { /* use max cpu */ } else //< @r-lyeh
        SDL_Delay(10);
        //continue;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

#if 1

    // Notifications: style setup + background color
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f); // Disable round borders
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f); // Disable borders
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.10f, 0.10f, 0.10f, 1.00f)); 
            ImGuiToast::RenderNotifications();
        ImGui::PopStyleColor(1);
    ImGui::PopStyleVar(2);

#endif

}

extern "C"
void sdl3_swap() {
    ImGuiIO& io = ImGui::GetIO();

    // Rendering
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
#if 0
    glClearColor(clear_color[0] * clear_color[3], clear_color[1] * clear_color[3], clear_color[2] * clear_color[3], clear_color[3]);
    glClear(GL_COLOR_BUFFER_BIT);
#endif

if( !ui_hidden() )

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, glContext) directly)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
        SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }

if( ui_frame > 3 ) // we consolidate UI widgets during first frames. do not draw those temporary frames.

    SDL_GL_SwapWindow(window);
}

extern "C"
void sdl3_quit(void) {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

extern "C"
void igTextWithHoverColor(ImU32 col, ImVec2 indents_offon, const char* text_begin) {
    using namespace ImGui;
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    const char *text_end = text_begin + strlen(text_begin);

    // Layout
    const ImVec2 text_pos(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
    const ImVec2 text_size = CalcTextSize(text_begin, text_end);
    ImRect bb(text_pos.x, text_pos.y, text_pos.x + text_size.x, text_pos.y + text_size.y);
    ItemSize(text_size, 0.0f);
    if (!ItemAdd(bb, 0))
        return;

    // Render
    bool hovered = IsItemHovered();
    if (hovered) PushStyleColor(ImGuiCol_Text, col);
    RenderText(bb.Min + ImVec2(hovered ? indents_offon.y : indents_offon.x,0), text_begin, text_end, false);
    if (hovered) PopStyleColor();
}

extern "C"
int igCurrentWindowStackSize(void) {
    using namespace ImGui;
    return GImGui->CurrentWindowStack.Size;
}

AUTORUN {
    if (!SDL_Init(~0u))
        die(SDL_GetError());
}
