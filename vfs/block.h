#pragma once
#include <cstdint>
class Block
{
	uint_fast32_t _id;
	char* _data;

public:
	Block();
	Block(int id, int size);

	char* data();

	char operator[](int index);
	void set_char(int index, char ch);
};

