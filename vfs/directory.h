#pragma once
#include "file.h"
#include "dentry.h"
#include <vector>
#include "path.h"

class Directory
{
	int						_parent_id;
	int						_inode_id;
	std::vector<DEntry*>	_dentries;

	static bool				_ROOT_EXISTS;

public:
	Directory(INode*);
	Directory(INode*, std::string name);
	Directory(char* content);
	Directory(INode* inode, std::string name, INode* parent);

	int parent()					{ return _parent_id; }
	int inode_id()					{ return _inode_id; }
	std::vector<DEntry*> dentry()	{ return _dentries; }

	void add(DEntry* dentry);
	void remove(int);

	char* ConvertToChar();
	int CharSize();
};

