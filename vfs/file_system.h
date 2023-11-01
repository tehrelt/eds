#pragma once
#include <string>
#include "service.h"

class FileSystem
{
	Service* _services;

public:
	FileSystem();
	FileSystem(Service* services);

	Service* services() { return _services; }

	Block* GetBlock(int id);
	Inode* GetInode(int id);

	static FileSystem* Create(std::string name, uint_fast64_t size);
	static FileSystem* Mount (std::string name);
};



