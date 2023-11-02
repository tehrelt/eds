#pragma once
#include "base_service.h"
#include "directory.h"

class DirectoryService : public BaseService
{
	void save_directory(Directory* directory);
	Directory* read_directory();
public:
	DirectoryService() : BaseService() { }
	DirectoryService(Storage* storage) : BaseService(storage) { }

	Directory* CreateRoot();
	Directory* ReadRoot();

	Directory* AddToDirectory(Directory* d, DEntry* dentry);

	std::vector<DEntry*> GetInfo(Directory* dir);
};

