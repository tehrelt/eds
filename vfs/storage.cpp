#include "storage.h"
#include <fstream>

void Storage::write(char* source, int offset, int size)
{
	std::fstream stream;

	stream.open(_file_name, std::ios::binary | std::ios::out | std::ios::in);

	if (!stream.is_open())
		throw new std::exception();


	stream.seekp(_first_block + offset, std::ios::beg);
	stream.write(source, size);

	stream.close();
}
void Storage::read(char* destination, int offset, int size)
{
	std::ifstream stream;

	stream.open(_file_name, std::ios::binary | std::ios::in);

	if (!stream.is_open()) {
		throw new std::exception();
	}

	stream.seekg(_first_block + offset, std::ios::beg);
	stream.read(destination, size);

	stream.close();
}

Storage::Storage(std::string name, Superblock* sb)
{
	_file_name = name;
	_superblock = sb;
	_first_block = 0;
	_block_size = sb->block_size();
}

Storage::~Storage()
{
	delete _superblock;
}
