#include "SystemTime.h"

#include <ctime>
#include <chrono>
#include <thread>

typedef std::chrono::duration<uint64_t, std::milli> milliseconds;

uint64_t SystemTime::getTimeInMillis() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<milliseconds>(now.time_since_epoch()).count();
}

double SystemTime::getTimeInSeconds() {
    return getTimeInMillis() / 1000.0;
}

void SystemTime::sleepInMillis(uint64_t millis) {
    std::this_thread::sleep_for(milliseconds(millis));
}
