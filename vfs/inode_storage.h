#pragma once
#include "storage.h"
#include "bitmap_storage.h"

#define INODE_SIZE sizeof(INode)

class INodeStorage : public Storage
{
private:
	BitmapStorage* _bitmap_storage;

public:
	INodeStorage(std::string name, Superblock* sb);

	INode&	allocate(Block& block);
	INode&	get		(int id);
	void	update	(INode& inode);
	void    free	(int id);
};

