#pragma once
#include "file.h"
#include "dentry.h"

class Directory
{
	Inode* _parent;
	Inode* _inode;
	DEntry* dentries;

	static bool _ROOT_EXISTS;

public:
	Directory(Inode*, std::string name);
	Directory(Inode* inode, std::string name, Inode* parent);

	DEntry* dentry();
	std::string GetPath();
};

