#pragma once
#include "repository.h"
#include "imap.h"
#include "superblock.h"

class INodeRepository : public Repository
{
private:
	IMap _imap;
	uint_fast32_t _part_location;
	uint_fast32_t _part_record_size;

	Inode* read_inode(int id);

	void save_bitmap_part(int id);

public:
	INodeRepository();
	INodeRepository(std::string name, Superblock* sb);
	INodeRepository(std::string name, Superblock* sb, IMap imap);

	void Save(Inode* inode);
	Inode* GetById(int id);
	Inode* FindFree();
	Inode* Allocate();
};

