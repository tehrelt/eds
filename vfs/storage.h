#pragma once
#include <string>
#include "superblock.h"
#include "imap.h"
#include "fat.h"
#include "block.h"

class Storage
{
private:
	std::string _file_name;

	Superblock _superblock;

	FAT _fat;
	IMap _imap;

	Block* read_block(int id);
	Inode* read_inode(int id);
	
	Inode* find_free_inode();
	Block* find_free_block();

	void write(char* source, int offset, int size);
	void read(char* destination, int offset, int size);

public:
	Storage();
	Storage(std::string name, Superblock* sb, FAT* fat, IMap* imap);

	std::string file_name()		{ return _file_name; }
	Superblock*	superblock()	{ return &_superblock; }
	FAT*		fat()			{ return &_fat; }
	IMap*		imap()			{ return &_imap; }

	int_fast32_t	get_fat_record(int i)	{ return _fat[i]; }
	Inode*			get_imap_record(int i)	{ return &_imap[i]; }

	Inode* AllocateInode();
	Inode* UpdateInode(Inode* inode);


	Block* GetBlock(int id);
};

