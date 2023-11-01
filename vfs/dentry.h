#pragma once
#include <string>
#include "inode.h"
#include "list_head.h"

class DEntry
{
	INode*	_inode;
	char	_name[16];

public:
	DEntry(INode* inode, std::string name);

	INode* inode()				{ return _inode; }
	uint_fast32_t inode_id()	{ return _inode->id(); }

	char* name()				{ return _name; }
};

