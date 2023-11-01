#pragma once
#include <string>
#include "inode.h"
#include "list_head.h"

class DEntry
{
	Inode*	_inode;
	char	_name[16];

public:
	DEntry(Inode* inode, std::string name);

	Inode* inode()				{ return _inode; }
	uint_fast32_t inode_id()	{ return _inode->id(); }

	char* name()				{ return _name; }
};

