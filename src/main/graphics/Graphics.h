#ifndef ENGINE_GRAPHICS_GRAPHICS_H_
#define ENGINE_GRAPHICS_GRAPHICS_H_

#include "datatypes.h"

class Window;

class Graphics {
public:
    static Graphics& getInstance();
    virtual ~Graphics() {};
    virtual bool init() = 0;
    virtual bool exit() = 0;
    virtual void setViewport() = 0;
    virtual void setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) = 0;
    virtual void setClearColor(Color color) = 0;
    virtual void setColor(Color color) = 0;
    virtual Color& getColor() = 0;
    virtual void clear() = 0;
    virtual bool handleErrors() = 0;
    virtual bool takeScreenshot(Window &window) = 0;
    virtual void setDepthTest(bool enable) = 0;
    virtual void pushState() = 0;
    virtual void popState() = 0;
protected:
    Graphics() {}
};

#endif /*ENGINE_GRAPHICS_GRAPHICS_H_*/
