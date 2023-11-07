#include "imap.h"
constexpr auto PART_SIZE = 64;

int IMap::get_part_index(int index)
{
	return (index) / (sizeof(uint_fast64_t) * 8);
}

IMap::IMap()
{ 
	_inodes = new INode[0];
	_capacity = 0;
	
	_parts_count = 0;
	_parts = new uint_fast64_t[0];
}
IMap::IMap(int capacity) 
{
	_capacity = capacity;
	_inodes = new INode[_capacity];

	_parts_count = (capacity + (8*sizeof(uint_fast64_t)) - 1) / (sizeof(uint_fast64_t) * 8);
	_parts = new uint_fast64_t[_parts_count];

	for (int i = 0; i < _parts_count; i++) {
		_parts[i] = 0;
	}
}

INode& IMap::operator[](int index) 
{ 
	if (index >= _capacity) {
		throw new imap_exception("Индекс за пределами массива");
	}
	return _inodes[index]; 
}



void IMap::Lock(int index)
{
	if (index >= _capacity) {
		throw new imap_exception("Индекс за пределами массива");
	}

	int part = get_part_index(index);
	int i = index % PART_SIZE;
	_parts[part] |= 1 << i;
}
void IMap::Unlock(int index)
{
	if (index >= _capacity) {
		throw new imap_exception("Индекс за пределами массива");
	}

	int part = get_part_index(index);
	int i = index % sizeof(uint_fast64_t);
	 _parts[part] &= ~(1 << i);
}
bool IMap::IsLocked(int index)
{
	if (index >= _capacity) {
		throw new imap_exception("Индекс за пределами массива");
	}

	int part = get_part_index(index);
	int i = index % (sizeof(uint_fast64_t) * 8);

	return (_parts[part] & 1 << i) != 0;
}

void IMap::set_inode(int index, INode* inode) 
{ 
	if (index >= _capacity) {
		throw new imap_exception("Индекс за пределами массива");
	}

	_inodes[index] = *inode;
}

uint_fast64_t IMap::part(int index) { return _parts[get_part_index(index)]; }
 
void IMap::set_part(int index, uint_fast64_t part)
{ 
	if (index >= _parts_count) {
		throw new imap_exception("Индекс за пределами массива _parts");
	}
	_parts[index] = part; 
}

uint_fast32_t IMap::capacity() { return _capacity; }
uint_fast32_t IMap::parts_count() {	return _parts_count; }

imap_exception::imap_exception(const std::string& message)
{
	_message = message;
}
const char* imap_exception::what() const noexcept
{
	return _message.c_str();
}
