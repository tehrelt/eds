#include "block_service.h"

Block* BlockService::Get(int id)
{
	return _storage->GetBlock(id);
}

Superblock* BlockService::GetSB()
{
	return _storage->superblock();
}

std::vector<int> BlockService::GetBlockchain(INode* inode)
{
	return _storage->GetBlockchain(inode->block_num());
}
