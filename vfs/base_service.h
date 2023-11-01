#pragma once
#include "storage.h"

class BaseService
{
protected:
	Storage* _storage;

public:
	BaseService();
	BaseService(Storage* storage);
};

