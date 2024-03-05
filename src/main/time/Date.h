#ifndef ENGINE_TIME_DATE_H_
#define ENGINE_TIME_DATE_H_

#include <stdint.h>

class Date {
public:
    Date();
    explicit Date(uint64_t epochInMillis);
    void setTime(uint64_t epochInMillis);
    uint64_t getTime() const;
private:
    uint64_t time;
};

#endif /*ENGINE_TIME_DATE_H_*/
