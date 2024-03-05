#include "Sync.h"

#include "time/Timer.h"

Timer *Sync::getTimer() {
    return timer;
}

void Sync::setTimer(Timer *timer) {
    this->timer = timer;
}
