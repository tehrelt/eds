#pragma once
#include <string>
#include <iostream>
#include "file_system.h"
#include <map>
#include <functional>

class execution_exception : public std::exception
{
private:
	std::string _message;
	std::string _command;
public:
	execution_exception(const std::string& message, const std::string& command);

	const char* what() noexcept;
};

class Terminal
{
	FileSystem* _file_system;
	

	Path path;

	std::map<std::string, std::function<void(std::vector<std::string> args, Directory* dir)>> _commands;
	
	void execute_command(const std::string& cmd);

	bool find_arg(std::vector<std::string> args, const std::string& arg);

	void mkfile(std::vector<std::string> args, Directory* dir);
	void mkdir(std::vector<std::string> args, Directory* dir);

	void rm(std::vector<std::string> args, Directory* dir);
	void rmdir(std::vector<std::string> args, Directory* dir);

	void sb(std::vector<std::string> args, Directory* dir);

	void get_block(std::vector<std::string> args, Directory* dir);
	void get_inode(std::vector<std::string> args, Directory* dir);
	void get_chain(std::vector<std::string> args, Directory* dir);

	void ls(std::vector<std::string> args, Directory* dir);

	void change_directory(std::vector<std::string> args, Directory* dir);

	void cat(std::vector<std::string> args, Directory* dir);

	void write(std::vector<std::string> args, Directory* dir);
	void write_append(std::vector<std::string> args, Directory* dir);

	void move(std::vector<std::string> args, Directory* dir);

	Directory* traverse_to_dir(std::string path);
	
	Path get_path();
	Path get_path(Directory* dir);

public:
	Terminal();
	Terminal(FileSystem* file_system);

	int Listen();

};

