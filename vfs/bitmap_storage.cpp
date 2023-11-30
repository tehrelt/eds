#include "bitmap_storage.h"

int BitmapStorage::get_map_index(int index)
{
	return (index) / (sizeof(uint_fast64_t) * 8);
}

BitmapStorage::BitmapStorage(std::string name, Superblock* sb) : Storage(name, sb)
{
	_first_block = sb->num_of_first_part_block();
	_capacity = sb->imap_capacity();
}

bool BitmapStorage::get(int id)
{
	int_fast64_t map = 0;
	int map_index = get_map_index(id);

	read((char*)&map, map_index * BITMAP_RECORD_SIZE, BITMAP_RECORD_SIZE);

	int i = id % (BITMAP_RECORD_SIZE * 8);

	return (map & 1 << i) != 0;
}

void BitmapStorage::set(int id)
{
	update(id, true);
}
void BitmapStorage::free(int id)
{
	update(id, false);
}

void BitmapStorage::update(int id, bool state)
{
	int_fast64_t map = 0;
	int map_index = get_map_index(id);
	read((char*)&map, map_index * BITMAP_RECORD_SIZE, BITMAP_RECORD_SIZE);

	int i = id % (BITMAP_RECORD_SIZE * 8);
	if (state) {
		map |= 1 << i;
	} else {
		map &= ~(1 << i);
	}

	write((char*)&map, map_index * BITMAP_RECORD_SIZE, BITMAP_RECORD_SIZE);
}

int BitmapStorage::find_free()
{
	for (int i = 0; i < _capacity; i++) {
		if (get(i) == false) {
			return i;
		}
	}
	throw std::exception("out of inodes");
}


