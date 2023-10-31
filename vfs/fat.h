#pragma once
#include <cstdint>

class FAT
{
	int_fast32_t* _records;
	uint_fast32_t _capacity;

public:
	FAT();
	FAT(int capacity);
	FAT(const FAT& other);

	uint_fast32_t capacity();

	int_fast32_t& operator[](int index);
};

