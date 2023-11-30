#include "inode_storage.h"
#include "tools.h"

INodeStorage::INodeStorage(std::string name, Superblock* sb) : Storage(name, sb)
{
    _first_block = sb->num_of_first_imap_block();
    _bitmap_storage = new BitmapStorage(name, sb);
}

INode& INodeStorage::allocate(Block& block)
{
	try
	{
		int free_id = _bitmap_storage->find_free();

		INode inode = INode(free_id);

		auto current_datetime = getCurrentDate();
		inode.set_mode(0b110100);
		inode.set_create_date(current_datetime);
		inode.set_modify_date(current_datetime);
		inode.set_access_date(current_datetime);
		inode.set_size(0);

		inode.set_block_num(block.id());

		_bitmap_storage->set(free_id);

		update(inode);
	}
	catch (const std::exception& e)
	{
		throw e;
	}
}

INode& INodeStorage::get(int id)
{
	INode inode = INode(id);

	read((char*)&inode, id * INODE_SIZE, INODE_SIZE);

	return inode;
}

void INodeStorage::update(INode& inode)
{
	write((char*)&inode, inode.id() * INODE_SIZE, INODE_SIZE);
}

void INodeStorage::free(int id)
{
	_bitmap_storage->free(id);
}
