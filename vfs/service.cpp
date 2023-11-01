#include "service.h"

Service::Service()
{
	_file_service = nullptr;
	_directory_service = nullptr;
}

Service::Service(FileSystem* file_system)
{
	_file_service = FileService(file_system);
	_directory_service = DirectoryService(file_system);
}
