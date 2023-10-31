#include "fat.h"
#include <exception>

FAT::FAT()
{
	_records = new int_fast32_t[0];
}

FAT::FAT(int capacity)
{
	_records = new int_fast32_t[capacity];
	_capacity = capacity;

	for (int i = 0; i < capacity; i++) {
		_records[i] = -1;
	}
}

FAT::FAT(const FAT& other)
{
	_records = other._records;
	_capacity = other._capacity;
}

uint_fast32_t FAT::capacity() { return _capacity; }
int_fast32_t& FAT::operator[](int index) {	return _records[index]; }
