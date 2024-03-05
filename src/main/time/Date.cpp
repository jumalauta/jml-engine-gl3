#include "Date.h"

#include "SystemTime.h"

Date::Date() {
    setTime(SystemTime::getTimeInMillis());
}

Date::Date(uint64_t epochInMillis) {
    setTime(epochInMillis);
}

void Date::setTime(uint64_t epochInMillis) {
    this->time = epochInMillis;
}

uint64_t Date::getTime() const {
    return time;
}
