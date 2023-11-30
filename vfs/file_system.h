#pragma once
#include <string>
#include "user.h"
#include "directory.h"

#define ROOT_INODE 0

class FileSystem
{
	

public:
	
	
	static FileSystem* Create(std::string name, uint_fast64_t size);
	static FileSystem* Mount (std::string name);
};