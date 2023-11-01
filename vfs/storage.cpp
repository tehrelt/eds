#include "storage.h"

Storage::Storage()
{
	_block_repository = BlockRepository();
	_inode_repository = INodeRepository();
}

Storage::Storage(std::string name, Superblock* sb)
{
	_block_repository = BlockRepository(name, sb);
	_inode_repository = INodeRepository(name, sb);
}

Storage::Storage(std::string name, BlockRepository block_repo, INodeRepository inode_repo)
{
	_inode_repository = inode_repo;
	_block_repository = block_repo;
}