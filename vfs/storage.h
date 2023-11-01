#pragma once
#include "block_repository.h"
#include "inode_repository.h"

class Storage
{
private:
	BlockRepository _block_repository;
	INodeRepository _inode_repository;
public:
	Storage(std::string name, Superblock* sb);
	BlockRepository block_repository() { return _block_repository; }
	INodeRepository inode_repository() { return _inode_repository; }
};

