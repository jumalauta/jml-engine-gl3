#ifndef ENGINE_UI_WINDOW_SDL_H_
#define ENGINE_UI_WINDOW_SDL_H_

#include "Window.h"

#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

class WindowSdl : public Window {
public:
    WindowSdl();
    bool init();
    bool exit();
    bool open();
    bool close();
    void swapBuffers();
    bool setTitle(std::string title, std::string extraInfo = "");
    bool setFullscreen(bool fullscreen);
    bool getFullscreen();
    bool bindGraphicsContext();
    void resize();
    void* getGraphicsContext();
    SDL_Window *window;

    void setFocus(bool focus);
    bool getFocus();
    void show(bool show);

private:
    void setOpenGlAttribute(SDL_GLattr attr, int value);
    bool setVerticalSync(bool vsync);
    void setCursor(bool cursor);

    bool fullscreen;
    bool focus;
    static SDL_GLContext graphicsContext;
};

#endif /*ENGINE_UI_WINDOW_SDL_H_*/
