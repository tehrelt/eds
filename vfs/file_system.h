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

	void ChangeDirectory(Directory* dir);
	void ChangeToRootDirectory();

	char* GetBlockContent(int inode_id);

	Directory* GetDirectory(int inode_id);
	Directory* GetParentDirectory();

	Block* GetBlock(int id);
	INode* GetInode(int id);

	File* CreateFile(std::string name);
	Directory* CreateDirectory(std::string name);

	void Write(int inode_id, std::string text);

	std::vector<DEntry*> ls();
	Superblock* sb();

	static FileSystem* Create(std::string name, uint_fast64_t size);
	static FileSystem* Mount (std::string name);
};



