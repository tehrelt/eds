#pragma once
#include "file.h"
#include "dentry.h"
#include <vector>
#include "path.h"

#define DIRECTORY_RECORD_CHAR_SIZE 16 + sizeof(int) + (sizeof(int) + 16)*_dentries.size()
#define DIRECTORY_ENTRY_SIZE sizeof(int) + 16

class Directory : public DEntry
{
	std::vector<DEntry*>	_dentries;
public:
	Directory(INode*);
	Directory(INode* inode, DEntry* parent, const std::string& name);

	std::vector<DEntry*> dentries()	{ return _dentries; }

	DEntry* exists(const std::string& name);

	void add(DEntry* dentry);

	void remove(int);
	void remove(DEntry* dentry);

	void clear();

	char* ToChar();
	int CharSize() { return DIRECTORY_RECORD_CHAR_SIZE; }
};

