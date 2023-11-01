#pragma once
#include "file.h"
#include "dentry.h"
#include <vector>

class Directory
{
	Inode*					_parent;
	Inode*					_inode;
	std::vector<DEntry*>	_dentries;
	int						_dentries_count;

	static bool				_ROOT_EXISTS;

public:
	Directory(Inode*, std::string name);
	Directory(Inode* inode, std::string name, Inode* parent);

	Inode* parent()					{ return _parent; }
	Inode* inode()					{ return _inode; }
	std::vector<DEntry*> dentry()	{ return _dentries; }
	int dentries_count()			{ return _dentries_count; }

	std::string GetPath();
};

