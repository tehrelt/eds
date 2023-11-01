#define _CRT_SECURE_NO_WARNINGS
#include "dentry.h"


DEntry::DEntry(INode* inode, std::string name)
{
	_inode = inode;
	strncpy(_name, name.c_str(), 16);
}
