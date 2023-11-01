#pragma once
#include <string>

#include "superblock.h"
#include "storage.h"

class FileSystem
{
	std::string _file_name;
	Superblock _superblock;
	Storage _storage;

public:
	FileSystem(std::string, Superblock);
	FileSystem(std::string, Superblock, Storage);

	//static FileSystem* Create(int size, std::string name);
	//static FileSystem* Mount(std::string name);

	std::string file_name()		{ return _file_name; }
	Superblock* superblock()	{ return &_superblock; }
	Storage* storage()			{ return &_storage; }
};

