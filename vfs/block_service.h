#pragma once
#include "base_service.h"

class BlockService : public	BaseService 
{
public:
	BlockService() : BaseService() {}
	BlockService(Storage* storage) : BaseService(storage) {}

	Block* GetBlock(int id);
};

