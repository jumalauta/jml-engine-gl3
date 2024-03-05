#ifndef ENGINE_TIME_FPS_H_
#define ENGINE_TIME_FPS_H_

#include <stdint.h>

class Fps {
public:
    Fps();
    void update();
    void setTargetFps(double targetFps);
    double getTargetFps();
    double getFps();
    double getTargetFpsSleepInMillis();
    uint64_t getTotalFrameCount();
    double getCurrentRenderTime();
private:
    uint64_t frameCount;
    uint64_t totalFrameCount;
    uint64_t startTime;
    double targetFps;
    double fps;
};

#endif /*ENGINE_TIME_FPS_H_*/
