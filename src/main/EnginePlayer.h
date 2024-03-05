#ifndef ENGINE_ENGINE_PLAYER_H_
#define ENGINE_ENGINE_PLAYER_H_

class Gui;
class Audio;
class Window;
class Input;
class Sync;
class Graphics;
class Timer;
class Fps;
class FileRefreshManager;
class NetworkManager;
class Fbo;
class TexturedQuad;
class ProgressBar;
class Camera;
class Shadow;
class MidiManager;
class Menu;

#include <functional>

#include "time/Timer.h"
#include "time/Fps.h"
#include "math/TransformationMatrixGlm.h"

enum class WindowType {
    CURRENT=0,
    EDITOR=1,
    PLAYER=2,
    MENU=3
};

/**
 * A glue layer for the main renderer. Intended to have timing, audio, i/o and gui.
 */
class EnginePlayer {
public:
    ~EnginePlayer();

    static EnginePlayer& getInstance();
    bool init();
    void run();
    bool exit();

    void forceRedraw();
    void forceReload();
    void setProgress(double progress);
    void setDrawFunction(const std::function<void()> &drawFunction);

    Audio& getAudio();
    Window* getWindow(WindowType type);
    Input& getInput();
    Sync& getSync();
    Graphics& getGraphics();
    FileRefreshManager& getFileRefreshManager();
    NetworkManager& getNetworkManager();
    MidiManager& getMidiManager();
    Timer& getTimer();
    Fps& getFps();
    Shadow& getShadow();
    void processFrame();
    void mainScreenDraw();

    Camera& getActiveCamera();
    void setActiveCamera(Camera& camera);
private:
    EnginePlayer();

    void toolGuiRender();
    bool load();

    void updateWindowTitle();

    Fbo *mainOutputFbo;
    TexturedQuad* mainOutputFboQuad;

    Gui* gui;
    Audio* audio;
    Window* editorWindow;
    Window* playerWindow;
    Menu* menu;
    Input* input;
    Sync* sync;
    Graphics* graphics;
    FileRefreshManager* fileRefreshManager;
    NetworkManager* networkManager;
    Timer timer;
    Fps fps;
    ProgressBar* progressBar;
    Shadow* shadow;
    MidiManager* midiManager;

    Camera *defaultCamera;
    Camera *activeCamera;

    double progress;

    bool redraw;
    bool reload;

    std::function<void()> drawFunction;
};

#endif /*ENGINE_ENGINE_PLAYER_H_*/
