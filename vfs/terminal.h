#pragma once
#include <string>
#include <iostream>
#include "file_system.h"
#include <map>
#include <functional>

class Terminal
{
	FileSystem* _file_system;
	

	Path path;

	std::map<std::string, std::function<void(std::vector<std::string> args)>> _commands;
	
	void execute_command(const std::string& cmd);
	bool find_arg(std::vector<std::string> args, const std::string& arg);
	bool exists(std::string name);

	void mkfile(std::vector<std::string> args);
	void mkdir(std::vector<std::string> args);
	void sb(std::vector<std::string> args);

	void get_block(std::vector<std::string> args);
	void get_inode(std::vector<std::string> args);

	void ls(std::vector<std::string> args);

	void change_directory(std::vector<std::string> args);

	void cat(std::vector<std::string> args);

public:
	Terminal();
	Terminal(FileSystem* file_system);

	int Listen();

};

