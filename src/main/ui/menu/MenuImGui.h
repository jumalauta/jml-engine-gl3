#ifndef ENGINE_UI_MENU_MENU_IM_GUI_H_
#define ENGINE_UI_MENU_MENU_IM_GUI_H_

#include "MenuSdl.h"

class MenuImGui : public MenuSdl {
public:
    bool init();
    bool exit();
    bool open();
    bool close();
    void render();
    unsigned int getWidth();
    unsigned int getHeight();
};

#endif /*ENGINE_UI_MENU_MENU_IM_GUI_H_*/
