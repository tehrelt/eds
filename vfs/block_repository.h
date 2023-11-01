#pragma once
#include "repository.h"
#include "fat.h"
#include "superblock.h"
#include "block.h"

class BlockRepository : public Repository
{
private:
	FAT _fat;

	int _fat_location;
	int _fat_record_size;

	Block* read_block(int id);

	void save_fat_record(int id);

public:
	BlockRepository();
	BlockRepository(std::string name, Superblock* sb);
	BlockRepository(std::string name, Superblock* sb, FAT fat);

	void Save(Block* block);
	Block* GetById(int id);
	Block* FindFree();
	Block* Allocate();
};

