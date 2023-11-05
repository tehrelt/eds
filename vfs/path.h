#pragma once
#include <vector>
#include <string>
#include "inode.h"

class Path
{
	std::vector<std::string> _parts;

public:
	Path();
	Path(std::string path);
	Path(std::vector<std::string> parts);

	void add(std::string part);
	void remove();
	Path reverse();
	
	std::string last();

	Path& operator+(const std::string& name);

	std::vector<std::string> parts() { return _parts; }
	std::string ToString();

	static std::string GetLastSegment(std::string path);
};

