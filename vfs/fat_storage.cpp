#include "fat_storage.h"

FATStorage::FATStorage(std::string name, Superblock* sb) : Storage(name, sb)
{ 
	_first_block = sb->num_of_first_fat_block();
	_capacity = sb->fat_capacity();
}

int& FATStorage::get(int id)
{
	int value = -3;

	read((char*)&value, id * FAT_RECORD_SIZE, FAT_RECORD_SIZE);

	return value;
}

void FATStorage::update(int id, int value)
{
	write((char*)&value, id * FAT_RECORD_SIZE, FAT_RECORD_SIZE);
}

int& FATStorage::find_free()
{
	for (int i = 0; i < _capacity; i++) {
		if (get(i) == -1) {
			return i;
		}
	}
	throw std::exception("out of free FAT records");
}

void FATStorage::free(int id)
{
	int current = get(id);
	update(id, EMPTY);

	while (current != EMPTY) {
		int temp = get(current);
		update(current, EMPTY);
		current = temp;
	}
}

