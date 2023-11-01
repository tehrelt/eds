#pragma once
#include <cstdint>
#include <iostream>

class Block
{
	uint_fast32_t _id;
	char* _data;

public:
	Block();
	Block(int id, int size);

	uint_fast32_t id() { return _id; }
	char* data();
	void set_data(char* data);

	char operator[](int index);
	void set_char(int index, char ch);

	friend std::ostream& operator<<(std::ostream& os, const Block& b);
};

