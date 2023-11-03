#pragma once
#include "base_service.h"

class InodeService : BaseService
{
public:
	InodeService() : BaseService() {}
	InodeService(Storage* storage) : BaseService(storage) {}

	INode* Get(int id);
	void SetOwner(INode* inode, int uid);
	void SetMode(INode* inode, uint_fast8_t mode);
	void Save(INode* inode);
};

