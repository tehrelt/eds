#pragma once
#include <cstdint>
#include <time.h>
#include <iostream>

constexpr uint_fast8_t SYSTEM = 0b0001;
constexpr uint_fast8_t HIDDEN = 0b0010;
constexpr uint_fast8_t ARCHIVE = 0b0100;
constexpr uint_fast8_t DIRECTORY = 0b1000;

class Inode
{

	int_fast32_t _id;
	uint_fast8_t _flags;
	uint_fast16_t _uid;
	uint_fast8_t _mode;
	time_t _create_date;
	time_t _modify_date;
	time_t _access_date;

	int_fast32_t _block_num;

public:
	Inode();
	Inode(int id);

	int_fast32_t id();
	uint_fast8_t flags();
	uint_fast16_t uid();
	uint_fast8_t mode();
	time_t create_date();
	time_t modify_date();
	time_t access_date();
	int_fast32_t block_num();

	void set_mode(uint_fast8_t mode);
	void set_flags(uint_fast8_t flags);

	void set_create_date(time_t date);
	void set_access_date(time_t date);
	void set_modify_date(time_t date);

	void set_block_num(int_fast32_t index);

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

	friend std::ostream& operator<<(std::ostream& os, const Inode& inode);
};

