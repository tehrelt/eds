#pragma once
#include "file_system.h"
#include "directory.h"

class DirectoryService
{
private:
	FileSystem* _file_system;

public:
	DirectoryService();
	DirectoryService(FileSystem* file_system);

	Directory Create(std::string name);
	Directory Update();
	void Delete();
};

