#pragma once
#include "storage.h"

#define BITMAP_RECORD_SIZE sizeof(int_fast64_t)

class BitmapStorage : public Storage
{
private:
	int _capacity;

	int get_map_index(int index);
	void	update(int id, bool state);

public:
	BitmapStorage(std::string name, Superblock* sb);

	bool	get			(int id);
	void	set			(int id);
	void	free		(int id);
	int		find_free	();
};

