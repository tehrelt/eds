#define _CRT_SECURE_NO_WARNINGS
#include "dentry.h"
#include "file.h"

DEntry::DEntry()
{
	_inode = nullptr;
	_parent = nullptr;
	strncpy(_name, "", 16);
	_path = Path();
}

DEntry::DEntry(INode* inode, DEntry* parent, const std::string& name)
{
	_inode = inode;
	_parent = parent;
	strncpy(_name, name.c_str(), 16);
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