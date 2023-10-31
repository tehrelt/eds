#pragma once
class Block
{
	char* _data;

public:
	Block();
	Block(int size);

	char* data();

	char operator[](int index);
	void set_char(int index, char ch);
};

