#pragma once
#include <vector>
#include <string>
#include "inode.h"

class Path
{
	std::vector<std::string> _parts;
	bool _is_absolute;

public:
	Path();
	Path(std::string path);
	Path(std::vector<std::string> parts);

	void push(std::string part);
	void pop();
	void remove();
	Path reverse();
	
	std::string last();

	Path& operator+(const std::string& name);

	std::vector<std::string> parts() { return _parts;		}
	bool is_absolute()				 { return _is_absolute; }
	std::string ToString();

	static std::string GetLastSegment(std::string path);
};

