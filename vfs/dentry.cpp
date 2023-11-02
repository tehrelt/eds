#define _CRT_SECURE_NO_WARNINGS
#include "dentry.h"


DEntry::DEntry()
{
	this->_inode_id = -1;
}

DEntry::DEntry(INode* inode, std::string name)
{
	_inode_id = inode->id();
	strncpy(_name, name.c_str(), 16);
}

DEntry::DEntry(int inode_id, std::string name)
{
	_inode_id = inode_id;
	strncpy(_name, name.c_str(), 16);
}