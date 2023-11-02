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

	void add(std::string part);
	void remove();
	void erase();

	Path& operator+(const std::string& name);

	std::string ToString();
};

