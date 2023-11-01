#include "block_repository.h"

Block* BlockRepository::read_block(int id)
{
	Block* block = new Block(0, _record_size);
	Repository::read((char*)block, _location + _record_size * id, _record_size);
	return block;
}
void BlockRepository::save_fat_record(int id)
{
	int_fast32_t record = _fat[id];
	write((char*)&record, _fat_location + (id * _fat_record_size), _fat_record_size);
}

BlockRepository::BlockRepository() : Repository()
{
	this->_fat = FAT();
	this->_fat_location = 0;
	this->_fat_record_size = 0;
}
BlockRepository::BlockRepository(std::string name, Superblock* sb) 
	: Repository(name, sb->num_of_first_data_block()* sb->block_size(), sb->block_size())
{
	_fat = FAT(sb->fat_capacity());
	_fat_location = sb->num_of_first_fat_block() * sb->block_size();
	_fat_record_size = sizeof(int_fast32_t);
}
BlockRepository::BlockRepository(std::string name, Superblock* sb, FAT fat)
	: Repository(name, sb->num_of_first_fat_block()* sb->block_size(), sb->block_size())
{
	_fat = fat;
	_fat_location = sb->num_of_first_fat_block() * sb->block_size();
	_fat_record_size = sizeof(int_fast32_t);
}

void BlockRepository::Save(Block* block)
{
	write((char*)block, (block->id() * _record_size), _record_size);
}

Block* BlockRepository::GetById(int id)
{
	return read_block(id);
}
Block* BlockRepository::FindFree()
{
	for (int i = 0; i < _fat.capacity(); i++) {
		if (_fat[i] == -1) {
			return read_block(i);
		}
	}
	return nullptr;
}
Block* BlockRepository::Allocate()
{
	Block* freeBlock = FindFree();

	_fat[freeBlock->id()] = -2;
	save_fat_record(freeBlock->id());

	return freeBlock;
}
