#pragma once
#include <string>
#include "inode.h"
#include "dentry.h"

class File : public DEntry
{
	int position;
public:
	File(INode* inode, DEntry* parent, const std::string& name);

	void open(std::string path, uint_fast8_t flags);
	void close();
	bool isOpen();

	void write(const char* bytes, int length);
	void write(const char* bytes, int offset, int length);

	char* read();
	char* read(int length);
	char* read(int offset, int length);
	
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

