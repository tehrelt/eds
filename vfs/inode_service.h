#pragma once
#include "base_service.h"

class InodeService : BaseService
{
public:
	InodeService() : BaseService() {}
	InodeService(Storage* storage) : BaseService(storage) {}
};

