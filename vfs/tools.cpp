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

std::vector<std::string> split(const std::string& input, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(input);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }

    return result;
}

// trim from start
std::string& ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
std::string& rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
        std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
std::string& trim(std::string& s) {
    return ltrim(rtrim(s));
}