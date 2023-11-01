#pragma once
#include <string>
#include <iostream>
#include "file_explorer.h"

class Terminal
{

public:

	int Listen();

	int mkfile(std::string);
	int mkdir(std::string);
	int ls();
};

