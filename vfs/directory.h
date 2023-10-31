#pragma once
#include "file.h"

class Directory
{

public:
	void CreateDirectory(std::string name);
	void CreateFile(std::string name);
	void Start();
	bool Exists(std::string name);
	File Open(std::string name);
	void Move(std::string source, std::string destination);
	void RemoveFile();
	void First();
};

