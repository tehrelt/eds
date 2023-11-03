#include "block_service.h"

Block* BlockService::Get(int id)
{
	return _storage->GetBlock(id);
}

Superblock* BlockService::GetSB()
{
	return _storage->superblock();
}

void BlockService::Write(INode* inode, std::string text)
{
	_storage->ClearBlocks(inode);
	_storage->WriteBytes(inode, 0, text.c_str(), text.size());
}

std::vector<int> BlockService::GetBlockchain(INode* inode)
{
	return _storage->GetBlockchain(inode->block_num());
}
