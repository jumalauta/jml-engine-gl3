#ifndef ENGINE_UI_GUI_SDL_H_
#define ENGINE_UI_GUI_SDL_H_

#include "Gui.h"

class GuiSdl : public Gui {
public:
    GuiSdl();
    ~GuiSdl();
    bool init();
    bool exit();
    void setCursorVisible(bool visible);
private:
    bool initialized;
};

#endif /*ENGINE_UI_GUI_SDL_H_*/
