#include "imap.h"
constexpr auto PART_SIZE = 64;

int Bitmap::get_part_index(int index)
{
	return (index) / (sizeof(uint_fast64_t) * 8);
}

Bitmap::Bitmap()
{ 
	_capacity = 0;
	
	_maps_count = 0;
	_maps = new uint_fast64_t[0];
}
Bitmap::Bitmap(int capacity) 
{
	_capacity = capacity;

	_maps_count = (capacity + (8*sizeof(uint_fast64_t)) - 1) / (sizeof(uint_fast64_t) * 8);
	_maps = new uint_fast64_t[_maps_count];

	for (int i = 0; i < _maps_count; i++) {
		_maps[i] = 0;
	}
}

INode& Bitmap::operator[](int index) 
{ 
	if (index >= _capacity) {
		throw new imap_exception("Индекс за пределами массива");
	}
	return _inodes[index]; 
}



void Bitmap::Lock(int index)
{
	if (index >= _capacity) {
		throw new imap_exception("Индекс за пределами массива");
	}

	int part = get_part_index(index);
	int i = index % PART_SIZE;
	_maps[part] |= 1 << i;
}
void Bitmap::Unlock(int index)
{
	if (index >= _capacity) {
		throw new imap_exception("Индекс за пределами массива");
	}

	int part = get_part_index(index);
	int i = index % PART_SIZE;
	 _maps[part] &= ~(1 << i);
}
bool Bitmap::IsLocked(int index)
{
	if (index >= _capacity) {
		throw new imap_exception("Индекс за пределами массива");
	}

	int part = get_part_index(index);
	int i = index % (sizeof(uint_fast64_t) * 8);

	return (_maps[part] & 1 << i) != 0;
}

void Bitmap::set_inode(int index, INode* inode) 
{ 
	if (index >= _capacity) {
		throw new imap_exception("Индекс за пределами массива");
	}

	_inodes[index] = *inode;
}

uint_fast64_t Bitmap::part(int index) { return _maps[get_part_index(index)]; }
 
void Bitmap::set_part(int index, uint_fast64_t part)
{ 
	if (index >= _maps_count) {
		throw new imap_exception("Индекс за пределами массива _parts");
	}
	_maps[index] = part; 
}

uint_fast32_t Bitmap::capacity() { return _capacity; }
uint_fast32_t Bitmap::parts_count() {	return _maps_count; }

imap_exception::imap_exception(const std::string& message)
{
	_message = message;
}
const char* imap_exception::what() const noexcept
{
	return _message.c_str();
}
