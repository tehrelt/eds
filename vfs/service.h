#pragma once
#include "file_service.h"
#include "directory_service.h"

class Service
{
private:
	FileService _file_service;
	DirectoryService _directory_service;

public:
	Service();
	Service(FileSystem* file_system);

	FileService file_service()				{ return _file_service; }
	DirectoryService directory_service()	{ return _directory_service; }
};

