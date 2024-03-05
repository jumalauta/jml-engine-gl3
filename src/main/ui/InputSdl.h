#ifndef ENGINE_UI_INPUT_SDL_H_
#define ENGINE_UI_INPUT_SDL_H_

#include "Input.h"

union SDL_Event;

class InputSdl : public Input {
public:
    InputSdl();
    ~InputSdl();
    bool init();
    bool exit();
    void setUserExit(bool userExit);
    bool isUserExit();
    void pollEvents();
    void processEvent();
protected:
    bool userExit;
    SDL_Event* event;
    bool ctrlPressed;
    bool shiftPressed;
};

#endif /*ENGINE_UI_INPUT_SDL_H_*/
