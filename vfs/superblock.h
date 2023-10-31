#pragma once
#include <cstdint>

class Superblock
{
private:	
	uint16_t _block_size;						// 2
	uint32_t _num_of_first_data_block;			// 4
	uint32_t _num_of_first_fat_block;			// 4
	uint32_t _num_of_first_imap_block;			// 4
	uint32_t _num_of_first_part_block;			// 4
	uint32_t _fat_capacity;						// 4
	uint32_t _fat_size;							// 4
	uint32_t _imap_capacity;					// 4
	uint32_t _imap_size;						// 4
	uint32_t _imap_parts_count;					// 4
	uint32_t _imap_parts_size;					// 4
	uint32_t _fs_size_in_blocks;				// 4
	uint32_t _data_blocks_count;				// 4
	uint64_t _free_space_in_bytes;				// 8
	uint64_t _total_space_in_bytes;				// 8

	char _fs_name[4];							// 4

public:
	Superblock();
	Superblock(uint_fast64_t size);
	Superblock(const Superblock& other);

	uint16_t block_size();
	uint32_t num_of_first_data_block();
	uint32_t num_of_first_fat_block();
	uint32_t num_of_first_imap_block();
	uint32_t num_of_first_part_block();
	uint32_t fat_capacity();
	uint32_t fat_size();
	uint32_t imap_capacity();	
	uint32_t imap_size();
	uint32_t imap_parts_count();
	uint32_t imap_parts_size();
	uint32_t fs_size_in_blocks();
	uint32_t data_blocks_count();
	uint64_t free_space_in_bytes();
	uint64_t total_space_in_bytes();

	char* fs_name();
};

