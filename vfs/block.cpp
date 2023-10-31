#include "block.h"

Block::Block()
{
	_data = new char[0];
}

Block::Block(int size)
{
	_data = new char[size];
}

char* Block::data() { return _data; }

char Block::operator[](int index) { return _data[index]; }
void Block::set_char(int index, char ch) { _data[index] = ch; }
