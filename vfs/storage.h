#pragma once
#include <string>
#include "superblock.h"
#include "imap.h"
#include "fat.h"
#include "block.h"
#include <vector>

class Storage
{
private:
	std::string _file_name;
protected:
	int _first_block;
	int _block_size;
	Superblock* _superblock;
	

	void write(char* source, int offset, int size);
	void read(char* destination, int offset, int size);

public:
	Storage(std::string name, Superblock* sb);
	~Storage();

	std::string file_name()		{ return _file_name; }
	Superblock*	superblock()	{ return _superblock; }
};

