#include <iostream>

#include "Superblock.h"
#include "fat.h"
#include "imap.h"

Superblock::Superblock()
{
	_block_size = 0;
	_num_of_first_data_block = 0;
	_num_of_first_fat_block = 0;
	_num_of_first_imap_block = 0;
	_num_of_first_part_block = 0;
	_fat_capacity = 0;
	_fat_size = 0;
	_imap_capacity = 0;
	_imap_size = 0;
	_imap_parts_count = 0;
	_imap_parts_size = 0;
	_data_blocks_count = 0;
	_fs_size_in_blocks = 0;
	_free_space_in_bytes = 0;
	_total_space_in_bytes = 0;
	_users_count = 0;
	strcpy_s(_fs_name, "");
}
Superblock::Superblock(uint_fast64_t size) : Superblock()
{
	if (size < 64 << 20)		{ _block_size = 512;  }
	else if (size < 128 << 20)  { _block_size = 1024; }
	else if (size < 256 << 20)  { _block_size = 2048; }
	else						{ _block_size = 4096; }

	int block_reserved = (sizeof(Superblock) + _block_size - 1) / _block_size;

	// FAT allocation
	_num_of_first_fat_block = block_reserved;
	_fat_capacity = size / _block_size;
	_fat_size = sizeof(uint_fast32_t) * _fat_capacity;
	block_reserved += (_fat_size + _block_size - 1) / _block_size;

	// INode list allocation
	_num_of_first_imap_block = block_reserved;
	_imap_capacity = size / 1000;
	_imap_size = sizeof(INode) * _imap_capacity;
	block_reserved += (_imap_size + _block_size - 1) / _block_size;
	
	// INode bitmap allocation
	_num_of_first_part_block = block_reserved;
	_imap_parts_count = (_imap_capacity + (8 * sizeof(uint_fast64_t)) - 1) / (sizeof(uint_fast64_t) * 8);
	_imap_parts_size = sizeof(uint_fast64_t) * _imap_parts_count;
	block_reserved += (_imap_parts_size + _block_size - 1) / _block_size;

	_num_of_first_data_block = block_reserved;

	_total_space_in_bytes = size - _block_size * block_reserved;
	_free_space_in_bytes = _total_space_in_bytes;
	_data_blocks_count = _total_space_in_bytes / _block_size;

	_fs_size_in_blocks = size / _block_size;

	_users_count = 0;

	strcpy_s(_fs_name, "EDS");
}
Superblock::Superblock(const Superblock& other)
{
	_block_size = other._block_size;			
	_num_of_first_data_block = other._num_of_first_data_block;
	_num_of_first_fat_block = other._num_of_first_fat_block;
	_num_of_first_imap_block = other._num_of_first_imap_block;
	_num_of_first_part_block = other._num_of_first_part_block;
	_fat_capacity = other._fat_capacity;
	_fat_size = other._fat_size;
	_imap_capacity = other._imap_capacity;
	_imap_size = other._imap_size;
	_imap_parts_count = other._imap_parts_count;
	_imap_parts_size = other._imap_parts_size;
	_fs_size_in_blocks = other._fs_size_in_blocks;
	_data_blocks_count = other._data_blocks_count;
	_free_space_in_bytes = other._free_space_in_bytes;
	_total_space_in_bytes = other._total_space_in_bytes;
	_users_count = other._users_count;
	strcpy_s(_fs_name, other._fs_name);
}

uint16_t Superblock::block_size() { return _block_size; }
uint32_t Superblock::num_of_first_data_block() { return _num_of_first_data_block; }
uint32_t Superblock::num_of_first_fat_block() {	return _num_of_first_fat_block; }
uint32_t Superblock::num_of_first_imap_block() { return _num_of_first_imap_block; }
uint32_t Superblock::num_of_first_part_block() { return _num_of_first_part_block; }

uint32_t Superblock::fat_capacity()			{ return _fat_capacity; }
uint32_t Superblock::fat_size()				{ return _fat_size; }
uint32_t Superblock::imap_capacity()		{ return _imap_capacity; }
uint32_t Superblock::imap_size()			{ return _imap_size; }
uint32_t Superblock::imap_parts_count()		{ return _imap_parts_count; }
uint32_t Superblock::imap_parts_size()		{ return _imap_parts_size; }
uint32_t Superblock::fs_size_in_blocks()	{ return _fs_size_in_blocks; }
uint32_t Superblock::data_blocks_count()	{ return _data_blocks_count; }
uint64_t Superblock::free_space_in_bytes()	{ return _free_space_in_bytes; }
uint64_t Superblock::total_space_in_bytes()	{ return _total_space_in_bytes; }
uint32_t Superblock::users_count()			{ return _users_count; }

void Superblock::add_user() { _users_count++; }

Superblock& Superblock::operator++()
{
	_users_count++;
	return *this;
}

char* Superblock::fs_name() { return _fs_name; }
