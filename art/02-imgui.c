#include "engine.h"

int main() {
    // create app
    app_create(0.75, 0);

    // main loop
    while( app_swap() ) {

        // create ui window and put some widgets in it
        static int flags = UI_OPEN | UI_MENUS;
        if( ui_window("UI Window", &flags) ) {

            // display tray, and menu section
            if( ui_tray() ) {
                if( ui_menu("File") ) {
                    if (ui_case("Some menu item", 0)) {}
                    ui_menu_end();
                }
                ui_tray_end();
            }

            ui_button("hello");

            ui_window_end();
        }

        if( ui_panel("UI Panel", 0) ) {
            // showcase many widgets
            ui_demo(0);
            ui_panel_end();
        }

        // freestanding widgets
        static float f = 3.14159;
        if( ui_float("my float", &f) );
        if( ui_button("my button") );

        // can invoke external cimgui functions directly too
        {
            igText("This is some useful text.");               // Display some text (you can use a format strings too)

            static float f = 0.0f;
            igSliderFloat("float", &f, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_Logarithmic);            // Edit 1 float using a slider from 0.0f to 1.0f

            static int counter = 0;
            if (igButton("Button",ImVec2(0,0)))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            igSameLine(0,0);
            igText("counter = %d", counter);

            static float knob = 0.5;
            igKnob("My knob", &knob, 10);
        }

        // showcase a few 3rd/ imgui libraries
        static unsigned show_demo_window = 0;
        if(show_demo_window&0x01) igShowDemoWindow(NULL);
        if(show_demo_window&0x02) igShowImPlotDemoWindow(NULL);
        if(show_demo_window&0x04) igFileDialogDemo();
        if(show_demo_window&0x08) igNodeFlowDemo();
        if(show_demo_window&0x10) igSequencerDemo();
        if(show_demo_window&0x20) igGuizmoDemo();

        // create a global app menu
        if( ui_tray() ) {
            if( ui_menu(UI_ICON(SETTINGS) " View demos") ) {
                if( ui_case(/*"ICN|*/"ImGui demo"/*|"*/, 0)) show_demo_window ^= 0x01;
                if( ui_case(/*"ICN|*/"ImPlot demo"/*|"*/, 0)) show_demo_window ^= 0x02;
                if( ui_case(/*"ICN|*/"ImFileDialog demo"/*|"*/, 0)) show_demo_window ^= 0x04;
                igSeparator();
                if( ui_case(/*"ICN|*/"ImNodeFlow demo"/*|"*/, 0)) show_demo_window ^= 0x08;
                igSeparator();
                if( ui_case(/*"ICN|*/"ImNodeSequencer demo"/*|"*/, 0)) show_demo_window ^= 0x10;
                if( ui_case(/*"ICN|*/"ImGuizmo demo"/*|"*/, 0)) show_demo_window ^= 0x20;
                if( ui_case(/*"ICN|*/"Exit"/*|CTRL+Q"*/, 0)) exit(0);
                ui_menu_end();
            }
            igSeparator();
            igText("%5.2f fps", window_fps());
            ui_tray_end();
        }
    }

    return 0;
}
