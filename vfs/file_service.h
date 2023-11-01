#pragma once
#include "file_system.h"
#include "file.h"
#include "directory.h"

class FileService
{
private:
	FileSystem* _file_system;

public:
	FileService();
	FileService(FileSystem* file_system);

	File Create(std::string name, Directory*);
	File Read();
	File Update();
	void Delete();
};

