#pragma once
#include "base_service.h"
#include "directory.h"

class DirectoryService : public BaseService
{
	void		save_directory(Directory* directory);
	Directory*	read_directory(INode* inode, char* dir, Directory* parent);

public:
	DirectoryService() : BaseService() { }
	DirectoryService(Storage* storage) : BaseService(storage) { }

	Directory* CreateRoot();
	Directory* ReadRoot();

	Directory* Create(std::string name, Directory* parent);

	void AddToDirectory(Directory* d, DEntry* dentry);
	void RemoveFromDirectory(Directory* d, DEntry* dentry);

	std::vector<DEntry*> GetInfo(Directory* dir);

	Directory* Get(INode* inode, Directory* parent);
};

