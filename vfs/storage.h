#pragma once
#include <string>
#include "superblock.h"
#include "imap.h"
#include "fat.h"
#include "block.h"
#include <vector>

constexpr auto ROOT_INODE	= 0;
constexpr auto USERS_INODE	= 1;

class Storage
{
private:
	std::string _file_name;

	Superblock _superblock;

	FAT _fat;
	IMap _imap;

	Block* read_block(int id);
	INode* read_inode(int id);
	
	INode* find_free_inode();
	Block* find_free_block();

	void save_superblock();

	Block* find_relative_block(INode* inode, int pos);

	void save_block(Block* block);
	void save_inode(INode* inode);

	void lock_inode(INode* inode);
	void set_fat_record(int idx, int value);

	void write(char* source, int offset, int size);
	void read(char* destination, int offset, int size);

public:
	Storage();
	Storage(std::string name, Superblock* sb, FAT* fat, IMap* imap);

	std::string file_name()		{ return _file_name; }
	Superblock*	superblock()	{ return &_superblock; }
	FAT*		fat()			{ return &_fat; }
	IMap*		imap()			{ return &_imap; }

	int GetNextUserId();

	int_fast32_t	get_fat_record(int i)	{ return _fat[i]; }
	INode*			get_imap_record(int i)	{ return &_imap[i]; }

	INode* AllocateInode();
	Block* AllocateBlock();
	Block* AllocateBlock(int prev_block_id);

	void ClearBlocks(INode* inode);

	void SaveINode(INode* inode);
	void SaveUser(char* user);

	std::vector<int> GetBlockchain(int block_id);

	Block* GetBlock(int id);
	INode* GetINode(int id);

	char* ReadINodeContent(int inode_id);

	void WriteBytes(INode* inode, int pos, const char* content, int size);
	void WriteByte(INode* inode, int pos, char byte);

	int GetEOF(INode* inode);
};

