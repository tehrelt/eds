#define _CRT_SECURE_NO_WARNINGS
#include "tools.h"
#include <iostream>
#include <fstream>

std::string timeToString(std::time_t time) {
    char buffer[26];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&time));
    return std::string(buffer);
}

std::string timeToString(std::time_t time, const char* pattern) {
    char buffer[26];
    strftime(buffer, sizeof(buffer), pattern, localtime(&time));
    return std::string(buffer);
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

std::string join(std::vector<std::string> const& strings, std::string delim)
{
    std::stringstream ss;
    std::copy(strings.begin(), strings.end(),
        std::ostream_iterator<std::string>(ss, delim.c_str()));
    return ss.str();
}

bool isForbiddenName(const std::string& name)
{
    if (name.compare(".") == 0) {
        return true;
    } else if (name.compare("..") == 0) {
        return true;
    } else if (name.compare("/") == 0) {
        return true;
    } else if (name.find('/') != std::string::npos) {
        return true;
    }
    return false;
}

void Log::log(const std::string& message, LOG_LEVEL level)
{
    std::string color;
    time_t time = getCurrentDate();

    switch (level)
    {
    case DEBUG: color = "\x1b[0;32m"; break;   // zeleniy
    case INFO:  color = "\x1b[0;35m"; break;   // phioletoviy
    case WARN:  color = "\x1b[0;33m"; break;   // zheltiy
    case ERROR: color = "\x1b[0;31m"; break;   // krasniy
    }

    std::cout << color << "[" << timeToString(time) << "]" << message;
    if (IS_DEBUG) {
        std::cout << " at " << "[" << _component << "]";
    }
    std::cout << "\x1b[0m" << std::endl;

    std::ofstream out;
    out.open("logs/" + timeToString(LAUNCH_TIME, "%Y_%m_%d__%H_%M_%S") + ".log", std::ios_base::app);
    out << "[" << timeToString(time) << "]" << message;
    if (IS_DEBUG) {
        out << " at " << "[" << _component << "]";
    }
    out << "\n";
    out.close();
}

bool Log::IS_DEBUG = false;
time_t Log::LAUNCH_TIME = getCurrentDate();

void Log::debug(const std::string& message)
{
    if (!IS_DEBUG) {
        return;
    }

    std::string prefix = "[DEBUG]";
    Log::log(prefix + message, DEBUG);
}

void Log::info(const std::string& message)
{
    std::string prefix = "[INFO]";
    Log::log(prefix + message, INFO);
}

void Log::warn(const std::string& message)
{
    std::string prefix = "[WARN]";
    Log::log(prefix + message, WARN);
}

void Log::error(const std::string& message)
{
    std::string prefix = "[ERROR]";
    Log::log(prefix + message, ERROR);
}

Log& Log::with(const std::string& component)
{
    Log log = Log(component);
    return log;
}