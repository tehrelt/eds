#pragma once
#include <string>
#include "inode.h"
#include "list_head.h"

class DEntry
{
	Inode* _inode;
	char* _name;
	uint_fast32_t _name_length;
	
	list_head _items;

public:
	DEntry(Inode* inode, std::string name);
	void add(DEntry* dentry);
	list_head items();

};

