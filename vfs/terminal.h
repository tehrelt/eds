#pragma once
#include <string>
#include <iostream>
#include "file_system.h"
#include <map>
#include <functional>

class Terminal
{
	FileSystem* _file_system;

	std::map<std::string, std::function<void()>> _commands;
	
	void execute_command(const std::string& cmd);

	void mkfile();
	void mkdir();
	void sb();

	void get_block();
	void get_inode();

	void ls();

	void change_directory();

public:
	Terminal();
	Terminal(FileSystem* file_system);

	int Listen();

};

