#ifndef ENGINE_TIME_TIMEFORMATTER_H_
#define ENGINE_TIME_TIMEFORMATTER_H_

#include <string>

class Date;

class TimeFormatter {
public:
    explicit TimeFormatter(std::string pattern);
    Date parse(const std::string& text);
    std::string format(const Date& time);
private:
    std::string pattern;
    std::string originalPattern;
};

#endif /*ENGINE_TIME_TIMEFORMATTER_H_*/
