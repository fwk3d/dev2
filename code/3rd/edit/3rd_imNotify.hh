/**
 * @file ImGuiNotify.hpp
 * @brief A header-only library for creating toast notifications with ImGui.
 * 
 * Based on imgui-notify by patrickcjk
 * https://github.com/patrickcjk/imgui-notify
 * 
 * @version 0.0.31 by r-lyeh: simplified implementation
 * @version 0.0.3  by TyomaVader
 * @date 07.07.2024
 */

#ifndef IMGUI_NOTIFY
#define IMGUI_NOTIFY

#pragma once

#include <vector>			// Vector for storing notifications list
#include <string>
#include <chrono>			// For the notifications timed dissmiss
#include <functional>		// For storing the code, which executest on the button click in the notification
 

/**
 * CONFIGURATION SECTION Start
*/

#define NOTIFY_MAX_MSG_LENGTH				4096		// Max message content length
#define NOTIFY_PADDING_X					20.f		// Bottom-left X padding
#define NOTIFY_PADDING_Y					20.f		// Bottom-left Y padding
#define NOTIFY_PADDING_MESSAGE_Y			10.f		// Padding Y between each message
#define NOTIFY_FADE_IN_OUT_TIME				150			// Fade in and out duration
#define NOTIFY_DEFAULT_DISMISS				3000		// Auto dismiss after X ms (default, applied only of no data provided in constructors)
#define NOTIFY_OPACITY						0.8f		// 0-1 Toast opacity
#define NOTIFY_USE_SEPARATOR 				false 		// If true, a separator will be rendered between the title and the content
#define NOTIFY_USE_DISMISS_BUTTON			true		// If true, a dismiss button will be rendered in the top right corner of the toast
#define NOTIFY_RENDER_LIMIT					5			// Max number of toasts rendered at the same time. Set to 0 for unlimited

// Warning: Requires ImGui docking with multi-viewport enabled
extern "C" API int flag(const char*);
#define NOTIFY_RENDER_OUTSIDE_MAIN_WINDOW	(flag("--viewports"))		// If true, the notifications will be rendered in the corner of the monitor, otherwise in the corner of the main window

/**
 * CONFIGURATION SECTION End
*/

static const ImGuiWindowFlags NOTIFY_DEFAULT_TOAST_FLAGS = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;

enum class ImGuiToastType : uint8_t
{
    None,
    Success,
    Warning,
    Error,
    Info,
    COUNT
};

enum class ImGuiToastPhase : uint8_t
{
    FadeIn,
    Wait,
    FadeOut,
    Expired,
    COUNT
};

enum class ImGuiToastPos : uint8_t
{
    TopLeft,
    TopCenter,
    TopRight,
    BottomLeft,
    BottomCenter,
    BottomRight,
    Center,
    COUNT
};

/**
 * @brief A class for creating toast notifications with ImGui.
 */
struct ImGuiToast
{
    ImGuiToastType								type = ImGuiToastType::None;
    std::string									title;
    std::string									content;

    int											dismissTime = NOTIFY_DEFAULT_DISMISS;
    ImGuiWindowFlags                            flags = NOTIFY_DEFAULT_TOAST_FLAGS;

    std::chrono::system_clock::time_point		creationTime = std::chrono::system_clock::now();

    std::function<void()>						onButtonPress = nullptr; // A lambda variable, which will be executed when button in notification is pressed
    char 										buttonLabel[NOTIFY_MAX_MSG_LENGTH];

    /**
     * @brief Get the default title of the toast notification based on its type.
     * 
     * @return const char* The default title of the toast notification.
     */
    const char* getDefaultTitle()
    {
        if (!title.empty())
        {
            switch (type)
            {
            case ImGuiToastType::None:
                return nullptr;
            case ImGuiToastType::Success:
                return "Success";
            case ImGuiToastType::Warning:
                return "Warning";
            case ImGuiToastType::Error:
                return "Error";
            case ImGuiToastType::Info:
                return "Info";
            default:
                return nullptr;
            }
        }

        return title.c_str();
    };

    /**
     * @brief Get the color of the toast notification based on its type.
     * 
     * @return ImVec4 The color of the toast notification.
     */
    ImVec4 getColor()
    {
        switch (type)
        {
        case ImGuiToastType::None:
            return {255, 255, 255, 255}; // White
        case ImGuiToastType::Success:
            return {0, 255, 0, 255}; // Green
        case ImGuiToastType::Warning:
            return {255, 255, 0, 255}; // Yellow
        case ImGuiToastType::Error:
            return {255, 0, 0, 255}; // Error
        case ImGuiToastType::Info:
            return {0, 157, 255, 255}; // Blue
        default:
            return {255, 255, 255, 255}; // White
        }
    }

    /**
     * @brief Get the icon of the toast notification based on its type.
     * 
     * @return const char* The icon of the toast notification.
     */
    const char* getIcon() 
    {
        switch (type)
        {
        case ImGuiToastType::None:
            return nullptr;
        case ImGuiToastType::Success:
            return UI_ICON(CHECK_CIRCLE); // "(v)", ICON_FA_CIRCLE_CHECK; // Font Awesome 6
        case ImGuiToastType::Warning:
            return UI_ICON(WARNING);      // "/!\\",ICON_FA_TRIANGLE_EXCLAMATION; // Font Awesome 6
        case ImGuiToastType::Error:
            return UI_ICON(ERROR);        // "(!)", ICON_FA_CIRCLE_EXCLAMATION; // Font Awesome 6
        case ImGuiToastType::Info:
            return UI_ICON(SMS);          // "(i)", ICON_FA_CIRCLE_INFO; // Font Awesome 6
        default:
            return nullptr;
        }
    }

    /**
     * @brief Get the elapsed time in milliseconds since the creation of the object.
     * 
     * @return int64_t The elapsed time in milliseconds.
     * @throws An exception with the message "Unsupported platform" if the platform is not supported.
     */
    std::chrono::nanoseconds getElapsedTime() 
    {
        return std::chrono::system_clock::now() - creationTime;
    }

    /**
     * @brief Get the current phase of the toast notification based on the elapsed time since its creation.
     * 
     * @return ImGuiToastPhase The current phase of the toast notification.
     *         - ImGuiToastPhase::FadeIn: The notification is fading in.
     *         - ImGuiToastPhase::Wait: The notification is waiting to be dismissed.
     *         - ImGuiToastPhase::FadeOut: The notification is fading out.
     *         - ImGuiToastPhase::Expired: The notification has expired and should be removed.
     */
    ImGuiToastPhase getPhase()
    {
        const int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(getElapsedTime()).count();

        if (elapsed > NOTIFY_FADE_IN_OUT_TIME + dismissTime + NOTIFY_FADE_IN_OUT_TIME)
        {
            if( dismissTime <= 0 ) return ImGuiToastPhase::Wait; //< @r-lyeh
            return ImGuiToastPhase::Expired;
        } else 
        if (elapsed > NOTIFY_FADE_IN_OUT_TIME + dismissTime)
        {
            return ImGuiToastPhase::FadeOut;
        } else 
        if (elapsed > NOTIFY_FADE_IN_OUT_TIME)
        {
            return ImGuiToastPhase::Wait;
        } else
        {
            return ImGuiToastPhase::FadeIn;
        }
    }

    /**
     * Returns the percentage of fade for the notification.
     * @return The percentage of fade for the notification.
     */
    float getFadePercent()
    {
        const ImGuiToastPhase phase = getPhase();
        const int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(getElapsedTime()).count();

        if (phase == ImGuiToastPhase::FadeIn)
        {
            return ((float)elapsed / (float)NOTIFY_FADE_IN_OUT_TIME) * NOTIFY_OPACITY;
        } else 
        if (phase == ImGuiToastPhase::FadeOut)
        {
            return (1.f - (((float)elapsed - (float)NOTIFY_FADE_IN_OUT_TIME - (float)dismissTime) / (float)NOTIFY_FADE_IN_OUT_TIME)) * NOTIFY_OPACITY;
        }

        return 1.f * NOTIFY_OPACITY;
    }


    static std::vector<ImGuiToast>& notifications() {
        static std::vector<ImGuiToast> notifications;
        return notifications;
    }

    /**
     * Renders all notifications in the notifications vector.
     * Each notification is rendered as a toast window with a title, content and an optional icon.
     * If a notification is expired, it is removed from the vector.
     */
    static void RenderNotifications()
    {
        auto &notifications = ImGuiToast::notifications();

        using namespace ImGui;

        const ImVec2 mainWindowSize = GetMainViewport()->Size;

        float height = 0.f;

        for (size_t i = 0; i < notifications.size(); ++i)
        {
            ImGuiToast* currentToast = &notifications[i];

            // Remove toast if expired
            if (currentToast->getPhase() == ImGuiToastPhase::Expired)
            {
                notifications.erase(notifications.begin()+i);
                --i;
                continue;
            }

            #if NOTIFY_RENDER_LIMIT > 0
                if (i > NOTIFY_RENDER_LIMIT)
                {
                    continue;
                }
            #endif

            // Get icon, title and other data
            const char* icon = currentToast->getIcon();
            const char* title = currentToast->title.c_str();
            const char* content = currentToast->content.c_str();
            const char* defaultTitle = currentToast->getDefaultTitle();
            const float opacity = currentToast->getFadePercent(); // Get opacity based of the current phase

            // Window rendering
            ImVec4 textColor = currentToast->getColor();
            textColor.w = opacity;

            // Generate new unique name for this toast
            char windowName[50];
                std::snprintf(windowName, 50, "##TOAST%d", (int)i);

            //PushStyleColor(ImGuiCol_Text, textColor);
            SetNextWindowBgAlpha(opacity);

            if( NOTIFY_RENDER_OUTSIDE_MAIN_WINDOW ) {
                short mainMonitorId = static_cast<ImGuiViewportP*>(GetMainViewport())->PlatformMonitor;

                ImGuiPlatformIO& platformIO = GetPlatformIO();
                ImGuiPlatformMonitor& monitor = platformIO.Monitors[mainMonitorId];

                // Set notification window position to bottom right corner of the monitor
                SetNextWindowPos(ImVec2(monitor.WorkPos.x + monitor.WorkSize.x - NOTIFY_PADDING_X, monitor.WorkPos.y + monitor.WorkSize.y - NOTIFY_PADDING_Y - height), ImGuiCond_Always, ImVec2(1.0f, 1.0f));
            } else {
                // Set notification window position to bottom right corner of the main window, considering the main window size and location in relation to the display
                ImVec2 mainWindowPos = GetMainViewport()->Pos;
                SetNextWindowPos(ImVec2(mainWindowPos.x + mainWindowSize.x - NOTIFY_PADDING_X, mainWindowPos.y + mainWindowSize.y - NOTIFY_PADDING_Y - height), ImGuiCond_Always, ImVec2(1.0f, 1.0f));
            }

            // Set notification window flags
            if (!NOTIFY_USE_DISMISS_BUTTON && currentToast->onButtonPress == nullptr)
            {
                currentToast->flags = NOTIFY_DEFAULT_TOAST_FLAGS | ImGuiWindowFlags_NoInputs;
            }

            Begin(windowName, nullptr, currentToast->flags);

            // Render over all other windows
            BringWindowToDisplayFront(GetCurrentWindow());

            // Here we render the toast content
            {
                PushTextWrapPos(mainWindowSize.x / 3.f); // We want to support multi-line text, this will wrap the text after 1/3 of the screen width

                bool wasTitleRendered = false;

                // If an icon is set
                if ( icon && icon[0] )
                {
                    //Text(icon); // Render icon text
                    TextColored(textColor, "%s", icon);
                    wasTitleRendered = true;
                }

                // If a title is set
                if ( title && title[0] )
                {
                    // If a title and an icon is set, we want to render on same line
                    if (icon && icon[0])
                        SameLine();

                    Text("%s", title); // Render title text
                    wasTitleRendered = true;
                } else 
                if (defaultTitle && defaultTitle[0])
                {
                    if (icon && icon[0])
                        SameLine();

                    Text("%s", defaultTitle); // Render default title text (ImGuiToastType_Success -> "Success", etc...)
                    wasTitleRendered = true;
                }

                // If a dismiss button is enabled
                if (NOTIFY_USE_DISMISS_BUTTON)
                {
                    // If a title or content is set, we want to render the button on the same line
                    if (wasTitleRendered || (content && content[0]))
                    {
                        SameLine();
                    }

                    // Render the dismiss button on the top right corner
                    // NEEDS TO BE REWORKED
                    float scale = 0.8f;

                    if (CalcTextSize(content).x > GetContentRegionAvail().x)
                    {
                        scale = 0.8f;
                    }

                    SetCursorPosX(GetCursorPosX() + (GetWindowSize().x - GetCursorPosX()) * scale);

                    // If the button is pressed, we want to remove the notification
                    if (Button(UI_ICON(CLOSE))) // "X")) //ICON_FA_XMARK))
                    {
                        notifications.erase(notifications.begin()+i);
                        --i;
                    }
                }

                // In case ANYTHING was rendered in the top, we want to add a small padding so the text (or icon) looks centered vertically
                if (wasTitleRendered && (content && content[0]))
                {
                    SetCursorPosY(GetCursorPosY() + 5.f); // Must be a better way to do this!!!!
                }

                // If a content is set
                if (content && content[0])
                {
                    if (wasTitleRendered)
                    {
                        #if NOTIFY_USE_SEPARATOR
                            Separator();
                        #endif
                    }

                    Text("%s", content); // Render content text
                }

                // If a button is set
                if (currentToast->onButtonPress != nullptr)
                {
                    // If the button is pressed, we want to execute the lambda function
                    if (Button(currentToast->buttonLabel))
                    {
                        currentToast->onButtonPress();
                    }
                }

                PopTextWrapPos();
            }

            // Save height for next toasts
            height += GetWindowHeight() + NOTIFY_PADDING_MESSAGE_Y;

            // End
            End();
        }
    }
};


//> @r-lyeh
extern "C"
void igInsertNotification(char type, const char *title, const char *message, float timeout) {
    // o(k), (w)arn, (i)nfo, (e)rror
    ImGuiToast t = {
        type == 'w' ? ImGuiToastType::Warning :
        type == 'e' ? ImGuiToastType::Error :
        type == 'k' ? ImGuiToastType::Success : ImGuiToastType::Info,
        title, message ? message : "", (int)(timeout * 1000) };
    ImGuiToast::notifications().push_back(t);
}
//< @r-lyeh

#endif
