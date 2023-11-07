#pragma once
#include <string>
#include "inode.h"
#include "storage.h"
#include "path.h"

#define DENTRY		0
#define FILE		1
#define DIRECTORY	2

class DEntry
{
protected:
	INode*	 _inode;
	DEntry*  _parent;
	char	 _name[12];
	Path	 _path;


public:
	DEntry();
	DEntry(INode* inode, DEntry* parent, const std::string& name);

	INode* inode()		{ return _inode;	}
	DEntry* parent()	{ return _parent;	}
	char* name()		{ return _name;		}
	Path* path()		{ return &_path;	}


	virtual int getType() { return DENTRY; }
};

