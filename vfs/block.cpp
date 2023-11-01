#include "block.h"


Block::Block()
{
	_id = 0;
	_data = new char[0];
}

Block::Block(int id, int size)
{
	_id = id;
	_data = new char[size];
}

char* Block::data() { return _data; }

void Block::set_data(char* data)
{
	if (strlen(data) > strlen(_data)) {
		throw new std::exception("Размер устанавливаемого блока должен быть равен размеру блока");
	}
	_data = data;
}

char Block::operator[](int index) { return _data[index]; }
void Block::set_char(int index, char ch) { _data[index] = ch; }

std::ostream& operator<<(std::ostream& os, const Block& b)
{
	os << "id: " << b._id << std::endl;
	os << "data: " << b._data << std::endl;
	return os;
}
