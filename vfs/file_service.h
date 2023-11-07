#pragma once
#include "base_service.h"
#include "file.h"

class FileService : BaseService
{
	int get_eof(INode* inode);
public:
	FileService() : BaseService() { }
	FileService(Storage* storage) : BaseService(storage) { }

	File* Create(std::string name, DEntry* parent);
	
	void Remove(DEntry* dentry);

	void Write(File* dentry, std::string text);
	void Append(File* inode, std::string text);

	char* Read(File* file);
};

