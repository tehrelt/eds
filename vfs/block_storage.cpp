#include "block_storage.h"

BlockStorage::BlockStorage(std::string name, Superblock* sb) : Storage(name, sb)
{
	_first_block = sb->num_of_first_data_block();
	_fat_storage = new FATStorage(name, sb);
}

Block& BlockStorage::allocate()
{
	try
	{
		int id = _fat_storage->find_free();
		Block block = Block(id, _block_size);
		*_superblock -= _block_size;
		_fat_storage->update(id, BLOCKCHAIN_END);
		return block;
	}
	catch (const std::exception& e)
	{
		throw e;
	}
}
Block& BlockStorage::allocate(int prev)
{
	try
	{
		int id = _fat_storage->find_free();
		Block block = Block(id, _block_size);
		*_superblock -= _block_size;
		_fat_storage->update(prev, id);
		_fat_storage->update(id, BLOCKCHAIN_END);
		return block;
	}
	catch (const std::exception& e)
	{
		throw e;
	}
}
Block& BlockStorage::get(int id)
{
	Block block = Block(id, _block_size);
	read(block.data(), id * _block_size, _block_size);
	return block;
}
void BlockStorage::save(Block& b)
{
	write(b.data(), b.id() * _block_size, _block_size);
}

void BlockStorage::free(int id)
{
	_fat_storage->free(id);
}
