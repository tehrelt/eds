#pragma once
#include "file_system.h"
#include "file.h"

class FileService
{
private:
	FileSystem* _file_system;

public:
	FileService();
	FileService(FileSystem* file_system);

	File Create(std::string name);
	File Read();
	File Update();
	void Delete();
};

