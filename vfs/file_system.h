#pragma once
#include <string>

#include "superblock.h"
#include "fat.h"
#include "imap.h"
#include "block.h"
#include "terminal.h"
#include "directory.h"

class FileSystem
{
	std::string _file_name;
	Superblock _superblock;
	FAT _fat;
	IMap _imap;
	Terminal _terminal;
	Directory* _current_directory;
	

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

	std::string file_name();

	Superblock* superblock();
	FAT* fat();
	IMap* imap();

	int Run();

	void GetRoot();
	void GetSuperBlock();
	void GetUID();
	void GetInode();

	std::string ToString();
};

