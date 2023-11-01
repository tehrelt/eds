#include "directory_service.h"

DirectoryService::DirectoryService()
{
	_file_system = nullptr;
}

DirectoryService::DirectoryService(FileSystem* file_system)
{
	_file_system = file_system;
}
