#include "storage.h"

Storage::Storage(std::string name, Superblock* sb)
{
	_block_repository = BlockRepository(name, sb);
	_inode_repository = INodeRepository(name, sb);
}
