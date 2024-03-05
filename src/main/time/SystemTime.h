#ifndef ENGINE_TIME_SYSTEM_TIME_H_
#define ENGINE_TIME_SYSTEM_TIME_H_

#include <stdint.h>

class SystemTime {
public:
    static uint64_t getTimeInMillis();
    static double getTimeInSeconds();
    static void sleepInMillis(uint64_t millis);
};

#endif /*ENGINE_TIME_SYSTEM_TIME_H_*/
