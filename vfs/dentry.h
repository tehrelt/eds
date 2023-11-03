#pragma once
#include <string>
#include "inode.h"

class DEntry
{
	int		_inode_id;
	char	_name[16];

public:
	DEntry();
	DEntry(INode* inode, std::string name);
	DEntry(int inode_id, std::string name);

	uint_fast32_t inode_id()	{ return _inode_id; }
	char* name()				{ return _name; }
};

