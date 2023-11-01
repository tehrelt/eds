#pragma once
#include "repository.h"
#include "fat.h"
#include "superblock.h"
#include "block.h"

class BlockRepository : public Repository
{
private:
	FAT* _fat;

public:
	BlockRepository();
	BlockRepository(std::string name, Superblock* sb);
	BlockRepository(std::string name, Superblock* sb, FAT* fat);

	void Save(Block* block);
	Block* GetById(int id);
	Block* FindFree();
	Block* Allocate();
};

