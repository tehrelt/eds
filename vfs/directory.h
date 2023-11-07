#pragma once
#include "file.h"
#include "dentry.h"
#include <vector>
#include "path.h"

#define DIRECTORY_ENTRY_SIZE sizeof(int) + 12
#define DIRECTORY_RECORD_CHAR_SIZE 12 + sizeof(int) + (DIRECTORY_ENTRY_SIZE)*_dentries.size()

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

