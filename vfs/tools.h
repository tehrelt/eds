#pragma once
#include <ctime>
#include <chrono>
#include <string>
#include <vector>
#include <sstream>

std::string timeToString(std::time_t time);
time_t getCurrentDate();

std::vector<std::string> split(const std::string& input, char delimiter);
