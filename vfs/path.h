#pragma once
#include <vector>
#include <string>

class Path
{
	std::vector<std::string> _parts;

public:
	Path();
	Path(std::string path);

	void add(std::string part);

	std::string ToString();
};
