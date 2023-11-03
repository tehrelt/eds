#pragma once
#include "base_service.h"
#include <vector>

class BlockService : public	BaseService 
{
public:
	BlockService() : BaseService() {}
	BlockService(Storage* storage) : BaseService(storage) {}

	Block* Get(int id);

	Superblock* GetSB();

	void Write(INode* inode, std::string text);

	std::vector<int> GetBlockchain(INode* inode);
};

