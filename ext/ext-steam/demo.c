// steamworks demo
// - rlyeh, public domain

#include "engine.h"

int main() {
    steam_init(0); // steam_appid: 0 to use SpaceWar vanilla project from Valve

    app_create(0.75, 0);

    while( app_swap() ) {
        steam_tick();

        if( ui_panel("Steam", UI_OPEN) ) {
            ui_steam();

            if( ui_button("Clear trophy") ) steam_trophy("ACH_TRAVEL_FAR_ACCUM", 0);
            if( ui_button("Redeem trophy") ) steam_trophy("ACH_TRAVEL_FAR_ACCUM", 1);
            if( ui_button("Submit Screenshot") ) steam_screenshot();

            ui_panel_end();
        }
    }
}
