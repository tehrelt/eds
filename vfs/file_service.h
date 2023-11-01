#pragma once
#include "file_system.h"

class FileService
{
private:
	FileSystem* _file_system;

public:
	FileService();
	FileService(FileSystem* file_system);

	File Create(std::string name);
	File Update();
	void Delete();
};

