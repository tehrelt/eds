#include "block_service.h"

Block* BlockService::Get(int id)
{
	return _storage->GetBlock(id);
}

Superblock* BlockService::GetSB()
{
	return _storage->superblock();
}
