#pragma once
#include "base_service.h"
#include "file.h"

class FileService : BaseService
{
public:
	FileService() : BaseService() { }
	FileService(Storage* storage) : BaseService(storage) { }

	File* Create(std::string name);
	char* Read(int inode_id);
};

