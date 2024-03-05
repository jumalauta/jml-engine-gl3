#ifndef ENGINE_UI_INPUT_H_
#define ENGINE_UI_INPUT_H_

#include <map>
#include <string>

class Input {
public:
    static Input& getInstance();
    virtual ~Input() {};
    virtual bool init() = 0;
    virtual bool exit() = 0;
    virtual void setUserExit(bool userExit) = 0;
    virtual bool isUserExit() = 0;
    virtual void pollEvents() = 0;

    void rewindToStart();
    void rewindToEnd();
    void takeScreenshot();
    void rewind(double deltaSeconds);
    void toggleScreenLog();
    void toggleEditor();
    void toggleFullscreen();
    void togglePause();
    void refresh();
    void redraw();
    void resizeWindow();

    std::map<std::string, bool> getPressedKeyMap();

protected:
    Input();

    std::map<std::string, bool> pressedKeyMap;


};

#endif /*ENGINE_UI_INPUT_H_*/
