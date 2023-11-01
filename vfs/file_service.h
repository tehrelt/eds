#pragma once
#include "base_service.h"

class FileService : BaseService
{
public:
	FileService() : BaseService() { }
	FileService(Storage* storage) : BaseService(storage) { }
};

