#include "base_service.h"

BaseService::BaseService()
{
	_storage = nullptr;
}
BaseService::BaseService(Storage* storage)
{
	_storage = storage;
}
