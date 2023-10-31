#pragma once
#include <ctime>
#include <chrono>
#include <string>

std::string timeToString(std::time_t time);
time_t getCurrentDate();