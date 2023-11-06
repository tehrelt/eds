#pragma once
#include <string>
#include "inode.h"
#include "dentry.h"

class File : DEntry
{
	int position;
public:
	File(INode* inode, DEntry* parent, const std::string& name);

	void Open(std::string path, uint_fast8_t flags);
	void Close();

	void Write(char* bytes, int length);
	void Read(char* bytes, int length);
	
	void Seek(int offset, int start);
	int Tell();

	void Clear();
	INode* inode() { return _inode; }

	int Size();
	bool IsOpen();
	bool EndOfFile();
};

