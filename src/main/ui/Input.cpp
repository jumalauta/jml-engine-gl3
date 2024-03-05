#include "Input.h"

#include "EnginePlayer.h"
#include "Settings.h"
#include "io/FileRefreshManager.h"
#include "logger/logger.h"
#include "time/Timer.h"
#include "ui/Window.h"

Input::Input() {
    
}

std::map<std::string, bool> Input::getPressedKeyMap() {
    return pressedKeyMap;
}

void Input::rewindToStart() {
    double position = 0.0;

    Timer &timer = EnginePlayer::getInstance().getTimer();
    timer.setTimeInSeconds(position);

    redraw();
}

void Input::rewindToEnd() {
    double position = Settings::demo.length - 0.1;
    bool pause = true;

    Timer &timer = EnginePlayer::getInstance().getTimer();
    timer.setTimeInSeconds(position);
    timer.pause(pause);

    redraw();
}

void Input::takeScreenshot() {
    loggerError("FIXME - functionality not done yet");
    /*if (ctrlPressed && isPlayerEditor())
    {
        char filename[128];
        sprintf(filename, "demo_ss_%d.png", SDL_GetTicks());
        imageTakeScreenshot((const char*)filename);
    }*/
}

void Input::rewind(double deltaSeconds) {
    Timer &timer = EnginePlayer::getInstance().getTimer();
    double newTime = timer.getTimeInSeconds() + deltaSeconds;
    if (newTime < 0.0) {
        newTime = 0.0;
    } else if (newTime > Settings::demo.length) {
        newTime = Settings::demo.length - 0.1;
    }

    timer.setTimeInSeconds(newTime);

    redraw();

    //playerForceRedraw();
}

void Input::toggleScreenLog() {
    loggerError("FIXME - functionality not done yet");
}

void Input::toggleEditor() {
    Settings::gui.editor = ! Settings::gui.editor;

    Window* editorWindow = EnginePlayer::getInstance().getWindow(WindowType::EDITOR);
    if (editorWindow != NULL) {
        editorWindow->show(Settings::gui.editor);
    }

    if (!Settings::gui.editor) {
        Window* playerWindow = EnginePlayer::getInstance().getWindow(WindowType::PLAYER);
        if (playerWindow != NULL) {
            playerWindow->show(true);
        }
    }

    redraw();
}

void Input::toggleFullscreen() {
    EnginePlayer& enginePlayer = EnginePlayer::getInstance();
    Window* playerWindow = enginePlayer.getWindow(WindowType::PLAYER);
    if (playerWindow != NULL) {
        playerWindow->show(true);
        playerWindow->setFullscreen(! playerWindow->getFullscreen());
    }

    redraw();
}

void Input::togglePause() {
    Timer &timer = EnginePlayer::getInstance().getTimer();
    bool pause = ! timer.isPause();

    timer.pause(pause);

    redraw();
}

void Input::refresh() {
    loggerInfo("User requested reload");
    EnginePlayer& enginePlayer = EnginePlayer::getInstance();
    enginePlayer.getFileRefreshManager().forceReload();
}

void Input::redraw() {
    EnginePlayer& enginePlayer = EnginePlayer::getInstance();
    enginePlayer.forceRedraw();
}

void Input::resizeWindow() {
    Window* window = EnginePlayer::getInstance().getWindow(WindowType::CURRENT);
    if (window != NULL) {
        window->resize();
    }
}
