#pragma once
#include <string>
#include "inode.h"

class File
{
	INode* _inode;
public:
	File(INode* inode);

	void Open(std::string path, uint_fast8_t flags);
	void Close();

	void Write(char* bytes, int length);
	void Read(char* bytes, int length);
	
	void Seek(int offset, int start);
	int Tell();

	void Clear();
	int GetInode();

	int Size();
	bool IsOpen();
	bool EndOfFile();
};

