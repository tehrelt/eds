#pragma once
#include <string>
#include "inode.h"

class imap_exception : public std::exception
{
	std::string _message;
public:
	imap_exception(const std::string& message);
	const char* what() const noexcept override;
};

class IMap
{
	Inode* _inodes;
	uint_fast32_t _capacity;

	uint_fast64_t* _parts;
	uint_fast32_t _parts_count;


public:
	IMap();
	IMap(int capacity);

	Inode& operator[](int index);
	
	void Lock(int index);
	void Unlock(int index);
	bool IsLocked(int index);

	void set_inode(int index, Inode* inode);

	int get_part_index(int index);
	uint_fast64_t part(int index);
	void set_part(int index, uint_fast64_t part);

	uint_fast32_t capacity();
	uint_fast32_t parts_count();
};

