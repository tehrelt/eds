#pragma once
#include <string>
#include <iostream>
#include "service.h"

class Terminal
{
	Service* _services;
public:
	Terminal();
	Terminal(Service* services);

	int Listen();

	int mkfile(std::string);
	int mkdir(std::string);
	int ls();
};

