#include "block_repository.h"

BlockRepository::BlockRepository() : Repository()
{
	this->_fat = nullptr;
	this->_fat_location = 0;
	this->_record_size = 0;
}

BlockRepository::BlockRepository(std::string name, Superblock* sb) : Repository(name)
{
	_fat = new FAT(sb->fat_capacity());
	_fat_location = sb->num_of_first_fat_block() * sb->block_size();
	_record_size = sizeof(_fat[0]);
}
