#define _CRT_SECURE_NO_WARNINGS
#include "dentry.h"
#include "file.h"

DEntry::DEntry()
{
	_inode = nullptr;
	_parent = nullptr;
	strncpy(_name, "", 12);
	_path = Path();
}

DEntry::DEntry(INode* inode, DEntry* parent, const std::string& name)
{
	_inode = inode;
	_parent = parent;
	strncpy(_name, name.c_str(), 12);
	if (parent == nullptr) {
		_path = Path();
		_path.push("");
	}
	else {
		_path = parent->_path;

		if (name == "..") {
			_path.pop();
		}
		else {
			_path.push(name);
		}		
	}
}

void DEntry::set_name(const std::string& name)
{
	strncpy(_name, name.c_str(), 12);
}

void DEntry::set_mode(int mode)
{
	_inode->set_mode(mode);
}
