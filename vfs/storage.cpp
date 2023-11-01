#include "storage.h"

#include <fstream>

Block* Storage::read_block(int id)
{
	Block* block = new Block(id, _superblock.block_size());
	char* block_content = new char[_superblock.block_size()];

	read(block_content,
		id * _superblock.block_size(), 
		_superblock.block_size());

	block->set_data(block_content);

	return block;
}
Inode* Storage::read_inode(int id)
{
	Inode* inode = new Inode();

	read((char*)inode,
		(_superblock.num_of_first_imap_block() * _superblock.block_size()) + (id * sizeof(Inode)),
		sizeof(Inode));

	return inode;
}

void Storage::write(char* source, int offset, int size)
{
	std::fstream stream;

	stream.open(_file_name, std::ios::binary | std::ios::out | std::ios::in);

	if (!stream.is_open())
		throw new std::exception();


	stream.seekp(offset, std::ios::beg);
	stream.write(source, size);

	stream.close();
}
void Storage::read(char* destination, int offset, int size)
{
	std::ifstream stream;

	stream.open(_file_name, std::ios::binary | std::ios::in);

	if (!stream.is_open())
		throw new std::exception();

	stream.seekg(offset, std::ios::beg);
	stream.read(destination, size);

	stream.close();
}

Storage::Storage()
{
	_file_name = "";
	_fat = FAT();
	_imap = IMap();
	
	_superblock = Superblock();
}
Storage::Storage(std::string name, Superblock* sb, FAT* fat, IMap* imap)
{
	_file_name = name;
	_fat = *fat;
	_imap = *imap;

	_superblock = *sb;
}

Block* Storage::GetBlock(int id)
{
	if (id < _superblock.num_of_first_data_block()) {
		throw new std::exception("Нельзя просмотреть блоки зарезервированные под систему");
	}
	return read_block(id);
}

Inode* Storage::GetInode(int id)
{
	return read_inode(id);
}
