#pragma once
#include <string>
#include <iostream>

class Terminal
{
	std::string _current_path;
public:
	Terminal();
	int Listen();
};

