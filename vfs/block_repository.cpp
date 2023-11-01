#include "block_repository.h"

BlockRepository::BlockRepository() : Repository()
{
	this->_fat = nullptr;
}

BlockRepository::BlockRepository(std::string name, Superblock* sb) 
	: Repository(name, sb->num_of_first_fat_block()* sb->block_size(), sizeof(_fat[0]))
{
	_fat = new FAT(sb->fat_capacity());
}
