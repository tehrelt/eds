#pragma once
#include "dentry.h"
class DEntryFactory
{
public:
	static DEntry* CREATE(int type, INode* inode, DEntry* parent, const std::string& name);
};

