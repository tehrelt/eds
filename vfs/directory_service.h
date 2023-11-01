#pragma once
#include "file_system.h"
#include "directory.h"

class DirectoryService
{
private:
	FileSystem* _file_system;
	Repository* _repository;

public:
	DirectoryService();
	DirectoryService(FileSystem* file_system);

	Directory*	CreateRoot();

	Directory*	Create(std::string name);
	Directory*	Get(Directory* current);
	Directory*	Update();

	void		Delete();
};

