#pragma once
#include <string>
#include "inode.h"
#include "dentry.h"

class File : public DEntry
{
	int _position;
public:
	File(INode* inode, DEntry* parent, const std::string& name);

	void write(const char* bytes, int length);

	char* read();
	char* read(int length);
	
	void seek(int offset);
	void seek(int offset, int start);
	int tell();

	void clear();
	INode* inode() { return _inode; }

	int length();

	time_t creationAt();
	time_t modifiedAt();
	time_t accessedAt();

	bool eof();


	int getType() override { return FILE; }
};

