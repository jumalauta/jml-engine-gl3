#include "GuiSdl.h"

#include "Settings.h"
#include "logger/logger.h"

#include <iostream>
#include <string>
#include <vector>

#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

Gui& Gui::getInstance() {
    static GuiSdl gui = GuiSdl();
    return gui;
}

GuiSdl::GuiSdl() {
    initialized = false;
}

GuiSdl::~GuiSdl() {
    exit();
}

bool GuiSdl::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        loggerError("Error: %s\n", SDL_GetError());
        return false;
    }

    initialized = true;

    int displayCount = SDL_GetNumVideoDisplays();
    int display = 0;

    if (displayCount < 1) {
        loggerError("SDL_GetNumVideoDisplays returned: %i", displayCount);
        return false;
    }

    std::vector<DisplayMode> displayModes = std::vector<DisplayMode>();

    int displayModeCount = SDL_GetNumDisplayModes(display);
    for (int displayModeI = 0; displayModeI < displayModeCount; displayModeI++) {
        SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };
        if (SDL_GetDisplayMode(display, displayModeI, &mode) != 0) {
            loggerError("SDL_GetDisplayMode failed: %s", SDL_GetError());
            return false;
        }

        DisplayMode displayMode;
        displayMode.width = mode.w;
        displayMode.height = mode.h;

        if (!displayModes.empty()) {
            DisplayMode previousMode = displayModes.back();
            if (displayMode.width == previousMode.width && displayMode.height == previousMode.height) {
                continue;
            }
        }

        if (Settings::demo.graphics.canvasWidth < displayMode.width
            && Settings::demo.graphics.canvasHeight < displayMode.height) {
            loggerTrace("Computer has higher resolution than canvas: %dx%d", displayMode.width, displayMode.height);
            continue;
        }

        displayModes.push_back(displayMode);
    }

    if (!displayModes.empty()) {
        this->displayModes = displayModes;
    }
    /*for (DisplayMode &mode : this->displayModes) {
        loggerInfo("%ux%u", mode.width, mode.height);
    }*/

    return true;
}

bool GuiSdl::exit() {
    if (initialized) {
        SDL_Quit();
        initialized = false;
    }

    return true;
}

void GuiSdl::setCursorVisible(bool visible) {
    int toggle = SDL_ENABLE;
    if (!visible) {
        toggle = SDL_DISABLE;
    }

    SDL_ShowCursor(toggle);
}
