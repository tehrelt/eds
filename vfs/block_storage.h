#pragma once
#include "storage.h"
#include "fat_storage.h"

class BlockStorage : public Storage
{
private:
	FATStorage* _fat_storage;

public:
	BlockStorage(std::string name, Superblock* sb);

	Block&	allocate	();
	Block&	allocate	(int prev);
	Block&	get			(int id);
	void	save		(Block& b);
	void	free		(int id);
};

