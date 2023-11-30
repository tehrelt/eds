#pragma once
#include "storage.h"

#define FAT_RECORD_SIZE sizeof(int_fast32_t)

#define EMPTY -1
#define BLOCKCHAIN_END -2

class FATStorage : public Storage
{
	int _capacity;
public:
	FATStorage(std::string name, Superblock* sb);

	int&	get			(int id);
	void	update		(int id, int value);
	int&	find_free	();
	void	free		(int id);
};

