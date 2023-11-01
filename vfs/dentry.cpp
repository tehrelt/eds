#include "dentry.h"


DEntry::DEntry(Inode* inode, std::string name)
{
	_inode = inode;
	_name_length = name.length();
	strncpy(_name, name.c_str(), _name_length);
	list_head_init(&_items);
}

void DEntry::add(DEntry * dentry)
{
	list_head_append(&_items, &dentry->_items);
}

list_head DEntry::items()
{
	return _items;
}
