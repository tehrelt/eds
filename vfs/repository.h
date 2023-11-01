#pragma once
#include <string>
class Repository
{
protected:
	std::string _file_name;

	void write(char* source, int offset, int size);
	void read(char* destination, int offset, int size);

public:
	Repository(std::string file_name);
};

