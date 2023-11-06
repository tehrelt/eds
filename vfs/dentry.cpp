#define _CRT_SECURE_NO_WARNINGS
#include "dentry.h"


DEntry::DEntry()
{
	_inode = nullptr;
	_parent = nullptr;
	strncpy(_name, "", 16);
}

DEntry::DEntry(INode* inode, DEntry* parent, const std::string& name)
{
	_inode = inode;
	_parent = parent;
	strncpy(_name, name.c_str(), 16);
}