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
	DEntry** dentries;
	int  dentires_count;
};

class Directory
{
	int						_parent_id;
	int						_inode_id;
	std::vector<DEntry*>	_dentries;

	static bool				_ROOT_EXISTS;

	Path					_path;

public:
	Directory(INode*, std::string name);
	Directory(char* content);
	Directory(INode* inode, std::string name, INode* parent);

	int parent()					{ return _parent_id; }
	int inode_id()					{ return _inode_id; }
	std::vector<DEntry*> dentry()	{ return _dentries; }

	std::string path()				{ return _path.ToString(); }

	Dir Convert();
	char* ConvertToChar();
	int CharSize();
};

