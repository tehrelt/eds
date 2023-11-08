#pragma once
#include <ctime>
#include <chrono>
#include <string>
#include <vector>
#include <sstream>
#include <functional>

std::string timeToString(std::time_t time);
time_t getCurrentDate();

std::vector<std::string> split(const std::string& input, char delimiter);

// trim from start
std::string& ltrim(std::string& s);

// trim from end
std::string& rtrim(std::string& s);

// trim from both ends
std::string& trim(std::string& s);