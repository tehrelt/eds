#pragma once
#include "base_service.h"

class DirectoryService : public BaseService
{
public:
	DirectoryService() : BaseService() { }
	DirectoryService(Storage* storage) : BaseService(storage) { }
};

