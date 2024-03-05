#include "InputSdl.h"

#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include "EnginePlayer.h"
#include "ui/Window.h"
#include "ui/WindowSdl.h"

#include "logger/logger.h"
#include "Settings.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"

Input& Input::getInstance() {
    static InputSdl input = InputSdl();
    return input;
}

InputSdl::InputSdl() {
    ctrlPressed = false;
    shiftPressed = false;
    userExit = false;
    event = new SDL_Event();
}

InputSdl::~InputSdl() {
    delete event;
}

bool InputSdl::init() {
    return true;
}

bool InputSdl::exit() {
    return true;
}

void InputSdl::setUserExit(bool userExit) {
    this->userExit = userExit;
}

bool InputSdl::isUserExit() {
    return userExit;
}

void InputSdl::pollEvents() {
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_QUIT
                || (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE)) {
            userExit = true;
            loggerDebug("User requested exit");
        }

        processEvent();
    }
}

static WindowType getEventWindowType(SDL_Event* event) {
    if (event->type != SDL_WINDOWEVENT) {
        return WindowType::CURRENT;
    }

    Uint32 eventWindowId = event->window.windowID;


    WindowSdl* playerWindow = dynamic_cast<WindowSdl*>(EnginePlayer::getInstance().getWindow(WindowType::PLAYER));
    if (playerWindow != NULL && SDL_GetWindowID(playerWindow->window) == eventWindowId) {
        return WindowType::PLAYER;
    }

    WindowSdl* editorWindow = dynamic_cast<WindowSdl*>(EnginePlayer::getInstance().getWindow(WindowType::EDITOR));
    if (editorWindow != NULL && SDL_GetWindowID(editorWindow->window) == eventWindowId) {
        return WindowType::EDITOR;
    }

    WindowSdl* menuWindow = dynamic_cast<WindowSdl*>(EnginePlayer::getInstance().getWindow(WindowType::MENU));
    if (menuWindow != NULL && SDL_GetWindowID(menuWindow->window) == eventWindowId) {
        return WindowType::MENU;
    }

    return WindowType::CURRENT;
}

void InputSdl::processEvent() {
    ImGui_ImplSDL2_ProcessEvent(event);

    if (event->type == SDL_WINDOWEVENT) {
        WindowType windowType = getEventWindowType(event);
        if (event->window.event == SDL_WINDOWEVENT_CLOSE) {
            if (windowType == WindowType::EDITOR) {
                toggleEditor();
            } else {
                loggerInfo("Window closed. Exitting application.");
                event->type = SDL_QUIT;
                SDL_PushEvent(event);
            }
        }

        Window* affectedWindow = EnginePlayer::getInstance().getWindow(windowType);
        if (affectedWindow != NULL) {
            if (event->window.event == SDL_WINDOWEVENT_RESIZED || event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                affectedWindow->resize();
                EnginePlayer::getInstance().forceRedraw();
            } else if (event->window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                affectedWindow->setFocus(true);
            } else if (event->window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                affectedWindow->setFocus(false);
            } else if (event->window.event == SDL_WINDOWEVENT_MAXIMIZED) {
                if (windowType == WindowType::PLAYER) {
                    if (!affectedWindow->getFullscreen()) {
                        //TODO: can't get off from fullscreen if window is maximized... :)
                        //toggleFullscreen();
                    }
                }
            }
        }

    }

    if (Settings::gui.tool) {
        if (event->type == SDL_DROPFILE) {
            //FIXME: Some functionality needed
            loggerInfo("File dropped: '%s'", event->drop.file);
        }
    }

    /*
    FIXME: MOUSE IMPLEMENTATION
    int mouseX;
    int mouseY;
    Uint32 mouseButtonState = SDL_GetMouseState(&mouseX, &mouseY);
    if (mouseButtonState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
    } else if (mouseButtonState & SDL_BUTTON(SDL_BUTTON_MIDDLE)) {
        
    } else if (mouseButtonState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        
    }

    // https://wiki.libsdl.org/SDL_MouseButtonEvent
    if (event->type == SDL_MOUSEWHEEL && event->button.type = SDL_MOUSEBUTTONDOWN) {
    }

    if (event->type == SDL_MOUSEWHEEL) {
        // https://wiki.libsdl.org/SDL_MouseWheelEvent
        //event->wheel.x: the amount scrolled horizontally, positive to the right and negative to the left
        //event->wheel.y: the amount scrolled vertically, positive away from the user and negative toward the user
    }*/

    // Check for the first key press event
    // https://wiki.libsdl.org/SDL_TextInputEvent & https://wiki.libsdl.org/Tutorials/TextInput
    // 
    if(event->type == SDL_MOUSEWHEEL) {
        //loggerInfo("MOUSEWHEEL x:%d, y:%d", event->wheel.x, event->wheel.y);
    } else if (event->type == SDL_KEYDOWN && event->key.state == SDL_PRESSED && event->key.repeat == 0) {
        switch(event->key.keysym.sym) {
            //ctrl / shift handling
            case SDLK_LCTRL:
            case SDLK_RCTRL:
                ctrlPressed = true;
                pressedKeyMap["ctrl"] = true;
                break;
            case SDLK_RSHIFT:
            case SDLK_LSHIFT:
                shiftPressed = true;
                pressedKeyMap["shift"] = true;
                break;

            case SDLK_DELETE:
                pressedKeyMap["delete"] = true;
                break;

            //numbers
            case SDLK_1:
                pressedKeyMap["number_1"] = true;
                break;
            case SDLK_2:
                pressedKeyMap["number_2"] = true;
                break;
            case SDLK_3:
                pressedKeyMap["number_3"] = true;
                break;
            case SDLK_4:
                pressedKeyMap["number_4"] = true;
                break;
            case SDLK_5:
                pressedKeyMap["number_5"] = true;
                break;
            case SDLK_6:
                pressedKeyMap["number_6"] = true;
                break;
            case SDLK_7:
                pressedKeyMap["number_7"] = true;
                break;
            case SDLK_8:
                pressedKeyMap["number_8"] = true;
                break;
            case SDLK_9:
                pressedKeyMap["number_9"] = true;
                break;
            case SDLK_0:
                pressedKeyMap["number_0"] = true;
                break;

            case SDLK_w:
                pressedKeyMap["key_w"] = true;
                break;
            case SDLK_a:
                pressedKeyMap["key_a"] = true;
                break;
            case SDLK_s:
                pressedKeyMap["key_s"] = true;
                break;
            case SDLK_d:
                pressedKeyMap["key_d"] = true;
                break;

            case SDLK_DOWN:
                pressedKeyMap["key_down"] = true;
                break;
            case SDLK_UP:
                pressedKeyMap["key_up"] = true;
                break;
            case SDLK_LEFT:
                pressedKeyMap["key_left"] = true;
                break;
            case SDLK_RIGHT:
                pressedKeyMap["key_right"] = true;
                break;

            case SDLK_SPACE:
                pressedKeyMap["key_space"] = true;
                break;
            case SDLK_RETURN:
                pressedKeyMap["key_return"] = true;
                break;
            case SDLK_BACKSPACE:
                pressedKeyMap["key_backspace"] = true;
                break;

            default:
                //no default functionality
                break;
        }

        if (ctrlPressed && Settings::gui.tool) {
            switch(event->key.keysym.sym) {
                //Some special keys
                case SDLK_TAB:
                    toggleScreenLog();
                    break;
                case SDLK_HOME:
                    rewindToStart();
                    break;
                case SDLK_END:
                    rewindToEnd();
                    break;
                case SDLK_PAGEDOWN:
                    rewind(-10);
                    break;
                case SDLK_PAGEUP:
                    rewind(10);
                    break;
                case SDLK_p:
                    takeScreenshot();
                    break;

                //numbers
                case SDLK_1:
                    rewind(-1);
                    break;
                case SDLK_2:
                    rewind(1);
                    break;
                case SDLK_3:
                    togglePause();
                    break;
                case SDLK_5:
                    refresh();
                    break;

                //characters
                case SDLK_e:
                    toggleEditor();
                    break;
                case SDLK_f:
                    toggleFullscreen();
                    break;

                default:
                    //no default functionality
                    break;
            }
        }
    } else if (event->type == SDL_KEYUP) {
        switch(event->key.keysym.sym) {
            case SDLK_LCTRL:
            case SDLK_RCTRL:
                ctrlPressed = false;
                pressedKeyMap.erase("ctrl");
                break;
            case SDLK_RSHIFT:
            case SDLK_LSHIFT:
                shiftPressed = false;
                pressedKeyMap.erase("shift");
                break;

            case SDLK_DELETE:
                pressedKeyMap.erase("delete");
                break;

            //numbers
            case SDLK_1:
                pressedKeyMap.erase("number_1");
                break;
            case SDLK_2:
                pressedKeyMap.erase("number_2");
                break;
            case SDLK_3:
                pressedKeyMap.erase("number_3");
                break;
            case SDLK_4:
                pressedKeyMap.erase("number_4");
                break;
            case SDLK_5:
                pressedKeyMap.erase("number_5");
                break;
            case SDLK_6:
                pressedKeyMap.erase("number_6");
                break;
            case SDLK_7:
                pressedKeyMap.erase("number_7");
                break;
            case SDLK_8:
                pressedKeyMap.erase("number_8");
                break;
            case SDLK_9:
                pressedKeyMap.erase("number_9");
                break;
            case SDLK_0:
                pressedKeyMap.erase("number_0");
                break;

            case SDLK_w:
                pressedKeyMap.erase("key_w");
                break;
            case SDLK_a:
                pressedKeyMap.erase("key_a");
                break;
            case SDLK_s:
                pressedKeyMap.erase("key_s");
                break;
            case SDLK_d:
                pressedKeyMap.erase("key_d");
                break;

            case SDLK_DOWN:
                pressedKeyMap.erase("key_down");
                break;
            case SDLK_UP:
                pressedKeyMap.erase("key_up");
                break;
            case SDLK_LEFT:
                pressedKeyMap.erase("key_left");
                break;
            case SDLK_RIGHT:
                pressedKeyMap.erase("key_right");
                break;

            case SDLK_SPACE:
                pressedKeyMap.erase("key_space");
                break;
            case SDLK_RETURN:
                pressedKeyMap.erase("key_return");
                break;
            case SDLK_BACKSPACE:
                pressedKeyMap.erase("key_backspace");
                break;

            default:
                //no default functionality
                break;
        }
    }
}
