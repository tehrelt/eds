#pragma once
#include <string>
#include <iostream>
#include "file_system.h"

class Terminal
{
	FileSystem* _file_system;

public:
	Terminal();
	Terminal(FileSystem* file_system);

	int Listen();

	int mkfile(std::string);
	int mkdir(std::string);

	int get_block(int id);
	int get_inode(int id);

	int ls();
};

