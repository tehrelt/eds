#pragma once
#include "inode.h"
#include "list_head.h"

class DEntry
{
	Inode* _inode;
	char* _name;
	uint_fast32_t _name_length;
	
	struct list_head _items;

public:
	void add(Inode* inode);
};

