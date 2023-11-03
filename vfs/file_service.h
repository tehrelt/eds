#pragma once
#include "base_service.h"
#include "file.h"

class FileService : BaseService
{
	int get_eof(INode* inode);
public:
	FileService() : BaseService() { }
	FileService(Storage* storage) : BaseService(storage) { }

	File* Create(std::string name);

	void Write(INode* inode, std::string text);
	void Append(INode* inode, std::string text);

	char* Read(int inode_id);
};

