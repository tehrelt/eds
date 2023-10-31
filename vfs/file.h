#pragma once
#include <string>
class File
{
public:
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

