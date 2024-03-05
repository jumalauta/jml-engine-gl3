#ifndef ENGINE_UI_GUI_H_
#define ENGINE_UI_GUI_H_

#include <vector>

struct DisplayMode {
    DisplayMode() {};
    DisplayMode(unsigned int width, unsigned int height) {
        this->width = width;
        this->height = height;
    }
    unsigned int width;
    unsigned int height;
};

class Gui {
public:
    static Gui& getInstance();
    virtual ~Gui() {};
    virtual bool init() = 0;
    virtual bool exit() = 0;
    virtual void setCursorVisible(bool visible) = 0;
    std::vector<DisplayMode>& getDisplayModes() {
        return displayModes;
    }
protected:
    Gui() {};
    std::vector<DisplayMode> displayModes;
};

#endif /*ENGINE_UI_GUI_H_*/
