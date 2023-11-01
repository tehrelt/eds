#pragma once
#include "file.h"
#include "dentry.h"
#include <vector>
#include "path.h"

class Dir 
{
public:
	int  parent_id;
	int  inode_id;
	int* dentries;
	int  dentires_count;
};

class Directory
{
	INode*					_parent;
	INode*					_inode;
	std::vector<DEntry*>	_dentries;

	static bool				_ROOT_EXISTS;

	Path					_path;

public:
	Directory(INode*, std::string name);
	Directory(INode* inode, std::string name, INode* parent);

	INode* parent()					{ return _parent; }
	INode* inode()					{ return _inode; }
	std::vector<DEntry*> dentry()	{ return _dentries; }

	std::string path()				{ return _path.ToString(); }

	Dir Convert();
};

