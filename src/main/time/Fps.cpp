#include "Fps.h"

#include "SystemTime.h"
#include "Settings.h"

Fps::Fps() {
    frameCount = 0;
    totalFrameCount = 0;
    targetFps = Settings::demo.targetFps; // vsync might affect target FPS
    startTime = SystemTime::getTimeInMillis();
    fps = targetFps;
}

void Fps::update() {
    frameCount++;
    totalFrameCount++;

    uint64_t elapsedTime = SystemTime::getTimeInMillis() - startTime;
    if (elapsedTime >= 1000) {
        fps = frameCount / (elapsedTime / 1000.0);
        frameCount = 0;
        startTime = SystemTime::getTimeInMillis();
    }
}

double Fps::getTargetFpsSleepInMillis() {
    return 1000.0 / getTargetFps();
}

void Fps::setTargetFps(double targetFps) {
    this->targetFps = targetFps;
}

double Fps::getTargetFps() {
    return targetFps;
}

double Fps::getFps() {
    return fps;
}

uint64_t Fps::getTotalFrameCount() {
    return totalFrameCount;
}

double Fps::getCurrentRenderTime() {
    return getFps() / 1.0;
}
