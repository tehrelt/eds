#define _CRT_SECURE_NO_WARNINGS
#include "tools.h"

std::string timeToString(std::time_t time) {
    std::string s(30, '\0');
    std::strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
    return s;
}
time_t getCurrentDate() {
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}
