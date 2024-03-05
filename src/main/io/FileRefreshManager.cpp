#include "FileRefreshManager.h"
#include "MemoryManager.h"
#include "Settings.h"
#include "io/File.h"
#include "graphics/Shader.h"
#include "graphics/Image.h"
#include "graphics/model/Model.h"
#include "graphics/video/VideoFile.h"
#include "graphics/Font.h"
#include "audio/AudioFile.h"
#include "script/Script.h"
#include "time/SystemTime.h"
#include "ui/Window.h"
#include "logger/logger.h"
#include "EnginePlayer.h"

#include <thread>
#include <vector>

FileRefreshManager& FileRefreshManager::getInstance() {
    static FileRefreshManager fileRefreshManager;
    return fileRefreshManager;
}

FileRefreshManager::FileRefreshManager() {
    reload = false;
    running = false;
    stopping = false;
}

bool FileRefreshManager::start() {
    if (!Settings::gui.tool) {
        // No file refresh polling if not in the tool mode
        return true;
    }

    if (running) {
        if (!exit()) {
            loggerError("File refresh manager instance already running and could not stop");
            return false;
        }
    }

    std::thread refreshManagerThread(&FileRefreshManager::markModified, this);
    refreshManagerThread.detach();


    for(int i = 0; !running && i < 10; i++) {
        const uint64_t START_LOOP_SLEEP = 10;
        SystemTime::sleepInMillis(START_LOOP_SLEEP);
    }

    if (!running) {
        loggerError("Could not start file refresh manager thread");
        return false;
    }

    return true;
}

bool FileRefreshManager::exit() {
    loggerTrace("Stopping file refresh manager");

    stopping = true;

    // Thread exit grace period
    while(running) {
        const uint64_t EXIT_LOOP_SLEEP = 10;
        SystemTime::sleepInMillis(EXIT_LOOP_SLEEP);
    }

    return true;
}

void FileRefreshManager::reloadModified() {
    if (!isModified()) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex);

    uint64_t reloadStart = SystemTime::getTimeInMillis();

    Timer &timer = EnginePlayer::getInstance().getTimer();

    Window* window = EnginePlayer::getInstance().getWindow(WindowType::PLAYER);
    if (window != NULL) {
        window->setTitle(Settings::demo.title);
    }

    bool paused = timer.isPause();

    if (reload) {
        loggerInfo("Reloading %d file(s)", reloadFiles.size());

        if (!paused) {
            timer.pause(true);
        }
    }

    for(File *file : reloadFiles) {
        if (file == NULL) {
            loggerError("NULL file in reloading");
            continue;
        }
        loggerInfo("Reloading file '%s'. pointer: 0x%p", file->getFilePath().c_str(), file);

        file->load();
    }

    loggerDebug("Reloaded %d file(s) in %u ms", reloadFiles.size(), SystemTime::getTimeInMillis() - reloadStart);

    EnginePlayer::getInstance().forceRedraw();

    reloadFiles.clear();
    
    if (reload) {
        reload = false;


        if (!paused) {
            timer.pause(false);
        }
    }
}

bool FileRefreshManager::isModified() {
    std::lock_guard<std::mutex> lock(mutex);

    if (!reloadFiles.empty()) {
        return true;
    }

    return false;
}

void FileRefreshManager::forceReload() {
    if (reload) {
        loggerWarning("Reload in progress");
        return;
    }

    reload = true;

    if (markFilesForRefresh() == 0) {
        loggerWarning("No files marked for reload");
        reload = false;
    }
}


void FileRefreshManager::markModified() {
    //EASY_THREAD("FileRefreshManager");

    running = true;

    while(!stopping) {
        SystemTime::sleepInMillis(Settings::gui.fileRefreshThreadSleep);

        if (!reload) {
            markFilesForRefresh();
        }

    }

    running = false;
    loggerDebug("thread exiting");
}

void FileRefreshManager::markFileForRefresh(File& file) {
    std::lock_guard<std::mutex> lock(mutex);
    loggerInfo("Requesting refresh for file: %s", file.getFilePath().c_str());
    reloadFiles.push_back(&file);
}

size_t FileRefreshManager::markFilesForRefresh() {
    if (reloadFiles.size() > 0) {
        // last bunch of files have not yet been processed
        return 0;
    }

    //TODO - this makes more sense to put stuff to memory manager during initial load
    std::vector<File*> files;
    for (auto it : MemoryManager<Image>::getInstance().getResources()) {
        files.push_back(it.second);
    }
    for (auto it : MemoryManager<AudioFile>::getInstance().getResources()) {
        files.push_back(it.second);
    }
    for (auto it : MemoryManager<VideoFile>::getInstance().getResources()) {
        files.push_back(it.second);
    }
    for (auto it : MemoryManager<Font>::getInstance().getResources()) {
        files.push_back(it.second);
    }
    for (auto it : MemoryManager<Model>::getInstance().getResources()) {
        files.push_back(it.second);
    }
    for (auto it : MemoryManager<Shader>::getInstance().getResources()) {
        files.push_back(it.second);
    }
    for (auto it : MemoryManager<Script>::getInstance().getResources()) {
        files.push_back(it.second);
    }

    for(File *file : files) {
        if (stopping) {
            break;
        }

        SystemTime::sleepInMillis(1);

        if (file->getFileScope() != FileScope::CONSTANT && (reload || file->modified())) {
            loggerInfo("Marking file '%s' for refresh. pointer: 0x%p", file->getFilePath().c_str(), file);
            if (reloadFiles.size() == 0) {
                mutex.lock();
            }
            reloadFiles.push_back(file);
        }
    }

    if (reloadFiles.size() > 0) {
        mutex.unlock();
    }

    return reloadFiles.size();
}
