#pragma once
#include "repository.h"
#include "fat.h"
#include "superblock.h"
#include "block.h"

class BlockRepository : public Repository
{
private:
	FAT* _fat;
	uint_fast16_t _record_size;
	uint_fast32_t _fat_location;

public:
	BlockRepository();
	BlockRepository(std::string name, Superblock* sb);

	void Save(Block* block);
	Block* GetById(int id);
	Block* FindFree();
	Block* Allocate();
};

