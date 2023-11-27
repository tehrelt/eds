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

std::string join(std::vector<std::string> const& strings, std::string delim);

bool isForbiddenName(const std::string& name);

enum LOG_LEVEL { DEBUG, INFO, WARN, ERROR };

class Log 
{
	std::string _component;
	

	void log(const std::string& message, LOG_LEVEL level);
public:
	Log() {
		_component = "";
	}
	Log(const std::string& component) {
		_component = component;
	}


	void debug(const std::string&);
	void info(const std::string&);
	void warn(const std::string&);
	void error(const std::string&);

	static Log& with(const std::string& component);
	static bool IS_DEBUG;
	static time_t LAUNCH_TIME;
};