#pragma once
#include "repository.h"
#include "imap.h"
#include "superblock.h"

class INodeRepository : public Repository
{
private:
	IMap* _imap;
	uint_fast32_t _part_location;
	uint_fast32_t _part_record_size;

public:
	INodeRepository();
	INodeRepository(std::string name, Superblock* sb);

	void Save(Inode* inode);
	Inode* GetById(int id);
	Inode* FindFree();
	Inode* Allocate();
};

