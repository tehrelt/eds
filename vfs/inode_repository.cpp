#include "inode_repository.h"

INodeRepository::INodeRepository() : Repository()
{
	this->_imap = nullptr;
	this->_part_location = 0;
	this->_part_record_size = 0;
}

INodeRepository::INodeRepository(std::string name, Superblock* sb)
	: Repository(name, sb->num_of_first_imap_block() * sb->block_size(), sizeof(_imap[0]))
{
	_imap = new IMap(sb->imap_capacity());

	_part_location = sb->num_of_first_part_block() * sb->block_size();
	_part_record_size = sizeof(uint_fast64_t);
}

INodeRepository::INodeRepository(std::string name, Superblock* sb, IMap* imap) 
	: Repository(name, sb->num_of_first_imap_block()* sb->block_size(), sizeof(imap[0]))
{
	_imap = imap;
}
