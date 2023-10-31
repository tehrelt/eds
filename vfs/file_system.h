#pragma once
#include <string>

#include "superblock.h"
#include "fat.h"
#include "imap.h"
#include "block.h"
#include "terminal.h"

class FileSystem
{
	std::string _file_name;

	Superblock _superblock;
	FAT _fat;
	IMap _imap;
	Terminal terminal;
	

	void write(char* source, int offset, int size);
	void read(char* destination, int offset, int size);

	void saveBlock(Block block, int index);

	void saveInode(int index);
	void saveInode(Inode* inode);

	void saveFAT(int index);

	Inode* allocateInode();

	void allocateFile();
	Inode* allocateDir();

	void generateStruct();

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

