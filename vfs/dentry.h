#pragma once
#include <string>
#include "inode.h"

class DEntry
{
protected:
	INode*	_inode;
	DEntry* _parent;
	char	_name[12];

public:
	DEntry();
	DEntry(INode* inode, DEntry* parent, const std::string& name);

	INode* inode()		{ return _inode;	}
	DEntry* parent()	{ return _parent;	}
	char* name()		{ return _name;		}

};

