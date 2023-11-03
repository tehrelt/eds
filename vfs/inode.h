#pragma once
#include <cstdint>
#include <time.h>
#include <iostream>

constexpr uint_fast8_t SYSTEM = 0b0001;
constexpr uint_fast8_t HIDDEN = 0b0010;
constexpr uint_fast8_t ARCHIVE = 0b0100;
constexpr uint_fast8_t DIRECTORY = 0b1000;

class INode
{

	int_fast32_t _id;					// 4
	uint_fast8_t _flags;				// 1
	uint_fast16_t _uid;					// 2
	uint_fast8_t _mode;					// 1
	time_t _create_date;				// 8
	time_t _modify_date;				// 8
	time_t _access_date;				// 8

	int_fast32_t _block_num;			// 4
	uint_fast32_t _size;				// 4

public:
	INode();
	INode(int id);

	int_fast32_t id();
	uint_fast8_t flags();
	uint_fast16_t uid();
	uint_fast8_t mode();
	time_t create_date();
	time_t modify_date();
	time_t access_date();
	int_fast32_t block_num();
	uint_fast32_t size();

	void set_mode(uint_fast8_t mode);
	void set_flags(uint_fast8_t flags);
	void set_uid(uint_fast16_t uid);

	void set_create_date(time_t date);
	void set_access_date(time_t date);
	void set_modify_date(time_t date);

	void set_block_num(int_fast32_t index);
	void set_size(uint_fast32_t size);

	void SetSystemFlag();
	void SetHiddenFlag();
	void SetArchiveFlag();
	void SetDirectoryFlag();

	bool IsSystemFlag();
	bool IsHiddenFlag();
	bool IsArchiveFlag();
	bool IsDirectoryFlag();

	void UnsetSystemFlag();
	void UnsetHiddenFlag();
	void UnsetArchiveFlag();
	void UnsetDirectoryFlag();

	friend std::ostream& operator<<(std::ostream& os, const INode& inode);
};

