#include "TimeFormatter.h"

#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <regex>
#include <string>

#include "Date.h"

TimeFormatter::TimeFormatter(std::string pattern) {
    this->originalPattern = pattern;

    pattern = std::regex_replace(pattern, std::regex("yyyy"), "%Y");
    pattern = std::regex_replace(pattern, std::regex("yy"), "%y");
    pattern = std::regex_replace(pattern, std::regex("MM"), "%m");
    pattern = std::regex_replace(pattern, std::regex("dd"), "%d");
    pattern = std::regex_replace(pattern, std::regex("HH"), "%H");
    pattern = std::regex_replace(pattern, std::regex("mm"), "%M");
    pattern = std::regex_replace(pattern, std::regex("ss"), "%S");
    this->pattern = pattern;
}

Date TimeFormatter::parse(const std::string& text) {
    int parsedMillis = 0;
    std::string tmpPattern = pattern;
    std::size_t found = originalPattern.find("SSS");
    if (found != std::string::npos) {
        parsedMillis = stoi(text.substr(found, 3));
        
        //pretty format parsed millis to have 3 digits always
        std::string parsedMillisString = std::string("000") + std::to_string(parsedMillis);
        parsedMillisString = parsedMillisString.substr(parsedMillisString.size() - 3);

        tmpPattern = std::regex_replace(tmpPattern, std::regex("SSS"), parsedMillisString);
    }

    std::tm localTime = {};
    std::istringstream ss(text);
    ss >> std::get_time(&localTime, tmpPattern.c_str());
    if (ss.fail()) {
        std::cout << "Parse failed:" << text << "\n";
        return Date(0);
    } else {
        uint64_t timeInMillis = static_cast<uint64_t>(mktime(&localTime) * 1000 + parsedMillis);
        Date time = Date(timeInMillis);
        return time;
    }
}

std::string TimeFormatter::format(const Date& time) {
    uint64_t epochInMillis = time.getTime();
    std::time_t t = static_cast<std::time_t>(epochInMillis / 1000);
    std::tm *localTime = std::gmtime(&t);

    std::stringstream ss;
    ss << std::put_time(localTime, pattern.c_str());
    std::string formattedDate = std::string(ss.str());

    //pretty format parsed millis to have 3 digits always
    std::string parsedMillisString = std::string("000") + std::to_string(epochInMillis % 1000);
    parsedMillisString = parsedMillisString.substr(parsedMillisString.size() - 3);

    formattedDate = std::regex_replace(formattedDate, std::regex("SSS"), parsedMillisString);

    return formattedDate;
}
