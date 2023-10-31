#pragma once
#include <string>

#include "superblock.h"
#include "fat.h"
#include "imap.h"
#include "block.h"

class FileSystem
{
	std::string _file_name;

	Superblock _superblock;
	FAT _fat;
	IMap _imap;
	

	void write(char* source, int offset, int size);
	void read(char* destination, int offset, int size);

	void saveBlock(Block block, int index);
	void saveInode(int index);
	void saveFAT(int index);

	Inode* allocateInode(int index);
	void allocateFile();
	int findFreeBlockNum();
	int findFreeInodeNum();



public:
	FileSystem(std::string, Superblock*, FAT*, IMap*);

	static FileSystem* Create(int size, std::string name);
	static FileSystem* Mount(std::string name);

	void GetRoot();
	void GetSuperBlock();
	void GetUID();
	void GetInode();

	std::string ToString();
};

