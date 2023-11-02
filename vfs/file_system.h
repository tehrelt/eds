#pragma once
#include <string>
#include "service.h"

class FileSystem
{
	Service* _services;
	Directory* _root;
	Directory* _current_directory;

public:
	FileSystem();
	FileSystem(Service* services);

	Service* services()				{ return _services; }
	Directory* current_directory()	{ return _current_directory; }

	Block* GetBlock(int id);
	INode* GetInode(int id);

	File* CreateFile(std::string name);

	std::vector<DEntry*> ls();
	Superblock* sb();

	static FileSystem* Create(std::string name, uint_fast64_t size);
	static FileSystem* Mount (std::string name);
};



