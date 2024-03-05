#ifndef ENGINE_TIME_TIMER_H_
#define ENGINE_TIME_TIMER_H_

#include "Date.h"

class Audio;

class Timer {
public:
    Timer();
    void start();
    void pause(bool paused);
    bool isPause();
    void stop();
    void update();
    void synchronizeToAudio(Audio* audio);
    void setBeatsPerMinute(double beatsPerMinute);
    double getBeatsPerMinute();
    double getBeatsPerSecond();
    double getSecondsPerBeat();
    uint64_t getTimeInMilliseconds();
    void setTimeInSeconds(double seconds);
    void setTimeInBeats(double beats);
    double getTimeInSeconds();
    double getTimeInBeats();
    Date& getElapsedTime();
private:
    Date elapsedTime;
    Date pauseTime;
    Date startTime;
    Audio* audio;
    double beatsPerMinute;
    bool paused;
    int64_t deltaTime;
};

#endif /*ENGINE_TIME_TIMER_H_*/
