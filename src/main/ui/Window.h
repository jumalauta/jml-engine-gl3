#ifndef ENGINE_UI_WINDOW_H_
#define ENGINE_UI_WINDOW_H_

#include <string>

class Window {
public:
    static Window* newInstance();
    static void showMessageBox(int level, const char *title, const char *message);

    virtual ~Window() {}
    virtual bool init() = 0;
    virtual bool exit() = 0;
    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual void swapBuffers() = 0;
    virtual bool bindGraphicsContext() = 0;

    virtual bool setFullscreen(bool fullscreen) = 0;
    virtual bool getFullscreen() = 0;
    virtual bool setTitle(std::string title, std::string extraInfo = "");
    std::string getTitle();
    virtual void resize() = 0;
    virtual void* getGraphicsContext() = 0;

    virtual void setFocus(bool focus) = 0;
    virtual bool getFocus() = 0;
    virtual void show(bool show) = 0;

    void setDimensions(unsigned int width, unsigned int height);
    unsigned int getWidth();
    unsigned int getHeight();

protected:
    Window();

private:
    std::string title;
    std::string extraInfo;
    unsigned int width;
    unsigned int height;
};

#endif /*ENGINE_UI_WINDOW_H_*/
