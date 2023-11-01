#include "inode_repository.h"

Inode* INodeRepository::read_inode(int id)
{
	Inode* inode = new Inode();
	read((char*)inode, _location + id * _record_size, _record_size);
	return inode;
}

void INodeRepository::save_bitmap_part(int id)
{
	uint_fast32_t part_idx = _imap.get_part_index(id);
    uint_fast64_t part = _imap.part(id);
    write((char*)&part, _part_location + part_idx * _part_record_size, _part_record_size);
}

INodeRepository::INodeRepository() : Repository()
{
	this->_imap = IMap();
	this->_part_location = 0;
	this->_part_record_size = 0;
}

INodeRepository::INodeRepository(std::string name, Superblock* sb)
	: Repository(name, sb->num_of_first_imap_block() * sb->block_size(), sizeof(_imap[0]))
{
	_imap = IMap(sb->imap_capacity());

	_part_location = sb->num_of_first_part_block() * sb->block_size();
	_part_record_size = sizeof(uint_fast64_t);
}

INodeRepository::INodeRepository(std::string name, Superblock* sb, IMap imap) 
	: Repository(name, sb->num_of_first_imap_block()* sb->block_size(), sizeof(imap[0]))
{
	_imap = imap;
	_part_location = sb->num_of_first_part_block() * sb->block_size();
	_part_record_size = sizeof(uint_fast64_t);
}

void INodeRepository::Save(Inode* inode)
{
	write((char*)inode, _location + inode->id() * _record_size, _record_size);
}

Inode* INodeRepository::GetById(int id)
{
	return read_inode(id);
}

Inode* INodeRepository::FindFree()
{
	for (int i = 0; i < _imap.capacity(); i++) {
		if (!_imap.IsLocked(i)) {
			return read_inode(i);
		}
	}
	return nullptr;
}

Inode* INodeRepository::Allocate()
{
	Inode* free = FindFree();

	_imap.Lock(free->id());
	save_bitmap_part(free->id());

	return free;
}
