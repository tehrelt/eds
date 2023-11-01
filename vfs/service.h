#pragma once
#include "file_service.h"
#include "directory_service.h"

class Service
{
private:
	FileService _file_service;
	DirectoryService _directory_service;

protected:
	Directory* _current_directory;

public:
	Service();
	Service(FileSystem* file_system);

	FileService* file_service()				{ return &_file_service; }
	DirectoryService* directory_service()	{ return &_directory_service; }

	Directory* current_directory()			{ return _current_directory; }
	void set_directory(Directory* dir)		{ _current_directory = dir; }
};

