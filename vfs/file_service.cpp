#include "file_service.h"

FileService::FileService()
{
	_file_system = nullptr;
}

FileService::FileService(FileSystem* file_system)
{
	_file_system = file_system;
}
