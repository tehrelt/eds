#include "file_service.h"


FileService::FileService()
{
	_file_system = nullptr;
}

FileService::FileService(FileSystem* file_system)
{
	_file_system = file_system;
}

File FileService::Create(std::string name, Directory*)
{
	return File();
}
