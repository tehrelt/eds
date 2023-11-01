#include "block_service.h"

Block* BlockService::GetBlock(int id)
{
	return _storage->GetBlock(id);
}
