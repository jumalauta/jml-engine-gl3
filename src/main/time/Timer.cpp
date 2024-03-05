#include "Timer.h"

#include "Settings.h"
#include "logger/logger.h"
#include "SystemTime.h"
#include "audio/Audio.h"

Timer::Timer() {
    elapsedTime = Date(0);
    startTime = Date(0);
    pauseTime = Date(0);
    deltaTime = 0;
    beatsPerMinute = Settings::demo.beatsPerMinute;
    paused = false;
}

void Timer::start() {
    startTime = Date();
    deltaTime = 0;

    if (isPause()) {
        //in case pause was started before time was started
        pause(true);
    }

    setTimeInSeconds(0.0);

    update();
}

void Timer::pause(bool paused) {
    this->paused = paused;

    if (audio) {
        audio->pause(paused);
    }

    if (paused) {
        if (pauseTime.getTime() > 0) {
            return;
        }

        pauseTime.setTime(Date().getTime());
    } else {
        if (pauseTime.getTime() <= 0) {
            return;
        }

        deltaTime -= static_cast<int64_t>(Date().getTime() - pauseTime.getTime());
        pauseTime = Date(0);
    }

    loggerDebug("Timer pause: %s", paused ? "true" : "false");
}

bool Timer::isPause() {
    return paused;
}

void Timer::stop() {
    elapsedTime = Date(0);
    startTime = Date(0);
    pause(true);
}

void Timer::synchronizeToAudio(Audio* audio) {
    this->audio = audio;

    if (Settings::audio.timeSource) {
        if (this->audio) {
            loggerTrace("Timer synchronized to audio");
        } else {
            loggerTrace("Timer synchronization to audio removed");
        }
    }
}

void Timer::update() {
    uint64_t now = Date().getTime();
    if (isPause()) {
        now = pauseTime.getTime();
    }

    if (Settings::audio.timeSource) {
        elapsedTime.setTime(static_cast<uint64_t>(audio->getTimeInSeconds() * 1000.0));
    } else {
        elapsedTime.setTime(now - startTime.getTime() + deltaTime);
    }
}

void Timer::setTimeInSeconds(double seconds) {
    if (seconds < 0.0) {
        start();
    } else {
        if (audio) {
            audio->setPosition(seconds);
        }
        if (!Settings::audio.timeSource) {
            deltaTime += -elapsedTime.getTime() + static_cast<int64_t>(seconds * 1000.0);
        }
    }

    update();
}

uint64_t Timer::getTimeInMilliseconds() {
    return elapsedTime.getTime();
}

double Timer::getTimeInSeconds() {
    return getTimeInMilliseconds() / 1000.0;
}

void Timer::setBeatsPerMinute(double beatsPerMinute) {
    this->beatsPerMinute = beatsPerMinute;
}

double Timer::getBeatsPerMinute() {
    return beatsPerMinute;
}

double Timer::getBeatsPerSecond() {
    return getBeatsPerMinute() / 60.0;
}

double Timer::getSecondsPerBeat() {
    return 60.0 / getBeatsPerMinute();
}

void Timer::setTimeInBeats(double beats) {
    setTimeInSeconds(beats * getSecondsPerBeat());
}

double Timer::getTimeInBeats() {
    return getTimeInSeconds() * getBeatsPerSecond();
}

Date& Timer::getElapsedTime() {
    return elapsedTime;
}
