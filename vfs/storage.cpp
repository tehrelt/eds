#include "storage.h"
#include <fstream>
#include "tools.h"

Block* Storage::read_block(int id)
{
	Block* block = new Block(id, _superblock.block_size());
	block->set_char(0, '\0');
	read(block->data(),
		id * _superblock.block_size(), 
		_superblock.block_size());

	std::memcpy(block->data(), block->data(), _superblock.block_size());

	return block;
}
INode* Storage::read_inode(int id)
{
	INode* inode = new INode();

	read((char*)inode,
		(_superblock.num_of_first_imap_block() * _superblock.block_size()) + (id * sizeof(INode)),
		sizeof(INode));

	return inode;
}

INode* Storage::find_free_inode()
{
	for (int i = 0; i < _imap.capacity(); i++) {
		if (!_imap.IsLocked(i)) {
			return &_imap[i];
		}
	}
	return nullptr;
}
Block* Storage::find_free_block()
{
	for (int i = 0; i < _fat.capacity(); i++) {
		if (_fat[i] == -1) {
			return read_block(i);
		}
	}
	return nullptr;
}

Block* Storage::find_relative_block(INode* inode, int pos)
{
	int block_num = inode->block_num();
	while (pos >= _superblock.block_size()) {
		block_num = _fat[block_num];
		pos -= _superblock.block_size();
	}

	return read_block(block_num);
}

void Storage::save_superblock()
{
	write((char*)&_superblock, 0, sizeof(Superblock));
}
void Storage::save_block(Block* block)
{
	char* content = block->data();
	write(content, block->id() * _superblock.block_size(), _superblock.block_size());
}
void Storage::save_inode(INode* inode)
{
	write((char*)inode, 
		_superblock.num_of_first_imap_block() * _superblock.block_size() + inode->id() * sizeof(INode),
		sizeof(INode));
}

void Storage::lock_inode(INode* inode)
{
	_imap.Lock(inode->id());
	auto part = _imap.part(inode->id());
	auto part_idx = _imap.get_part_index(inode->id());
	write((char*)&part,
		_superblock.num_of_first_part_block() * _superblock.block_size() + part_idx * sizeof(part),
		sizeof(part));
}
void Storage::set_fat_record(int idx, int value)
{
	_fat[idx] = value;
	write((char*)&_fat[idx],
		_superblock.num_of_first_fat_block() * _superblock.block_size() + idx * sizeof(int_fast32_t),
		sizeof(int_fast32_t));
}

void Storage::write(char* source, int offset, int size)
{
	std::fstream stream;

	stream.open(_file_name, std::ios::binary | std::ios::out | std::ios::in);

	if (!stream.is_open())
		throw new std::exception();


	stream.seekp(offset, std::ios::beg);
	stream.write(source, size);

	stream.close();
}
void Storage::read(char* destination, int offset, int size)
{
	std::ifstream stream;

	stream.open(_file_name, std::ios::binary | std::ios::in);

	if (!stream.is_open())
		throw new std::exception();

	stream.seekg(offset, std::ios::beg);
	stream.read(destination, size);

	stream.close();
}

Storage::Storage()
{
	_file_name = "";
	_fat = FAT();
	_imap = IMap();
	
	_superblock = Superblock();
}
Storage::Storage(std::string name, Superblock* sb, FAT* fat, IMap* imap)
{
	_file_name = name;
	_fat = *fat;
	_imap = *imap;

	_superblock = *sb;
}

INode* Storage::AllocateInode()
{
	INode* inode = find_free_inode();
	Block* free_block = AllocateBlock();

	inode->set_create_date(getCurrentDate());
	inode->set_block_num(free_block->id());

	lock_inode(inode);

	save_inode(inode);

	return inode;
}

Block* Storage::AllocateBlock()
{
	Block* block = find_free_block();
	block->set_char(0, '\0');
	save_block(block);

	_superblock -= _superblock.block_size();

	set_fat_record(block->id(), -2);

	save_superblock();

	return block;
}
Block* Storage::AllocateBlock(int prev_id)
{
	Block* block = find_free_block();
	block->set_char(0, '\0');
	save_block(block);

	_superblock -= _superblock.block_size();

	set_fat_record(prev_id, block->id());
	set_fat_record(block->id(), -2);

	save_superblock();

	return block;
}

void Storage::ClearBlocks(INode* inode)
{
	Block* clear_block = new Block(0, _superblock.block_size());
	Block* block = nullptr;
	
	int id = inode->block_num();
	while (id != -2) {
		block = GetBlock(id);
		std::memcpy(block->data(), clear_block->data(), _superblock.block_size());
		
		save_block(block);

		id = _fat[id];
	}

	delete block;
	delete clear_block;

	id = inode->block_num();
	while (id != -2) {
		if (id != inode->block_num()) {
			_superblock += _superblock.block_size();
		}
		int tmp = id;
		id = _fat[id];
		set_fat_record(tmp, -1);
	}

	set_fat_record(inode->block_num(), -2);

	save_superblock();
}
void Storage::SaveINode(INode* inode)
{
	save_inode(inode);
}

std::vector<int> Storage::GetBlockchain(int block_id)
{
	int id = block_id;
	std::vector<int> chain = std::vector<int>();

	while (id != -2) {
		chain.push_back(id);
		id = _fat[id];
	}

	chain.push_back(-2);

	return chain;
}

Block* Storage::GetBlock(int id)
{
	if (id < _superblock.num_of_first_data_block()) {
		throw new std::exception("Ќельз€ просмотреть блоки зарезервированные под систему");
	}
	else if (id >= _superblock.fat_capacity()) {
		throw new std::exception("«аданный id уходит за пределы FAT");
	}

	return read_block(id);
}

INode* Storage::GetINode(int id)
{
	if (id >= _superblock.imap_capacity()) {
		throw new std::exception("«аданный id уходит за пределы IMap");
		return nullptr;
	}
	return read_inode(id);
}

char* Storage::ReadINodeContent(int inode_id)
{

	INode* inode = GetINode(inode_id);
	char* content = new char[(GetBlockchain(inode->block_num()).size() - 1) * _superblock.block_size()];

	int p = 0;
	int id = inode->block_num();
	while (id != -2) {

		char* bc = new char[_superblock.block_size()];

		read(bc, id * _superblock.block_size(), _superblock.block_size());

		std::memcpy(content + p, bc, _superblock.block_size());

		p += _superblock.block_size();

		id = _fat[id];

		delete[] bc;
	}

	return content;
}

void Storage::WriteBytes(INode* inode, int pos, const char* content, int size)
{
	Block* block = find_relative_block(inode, pos);
	
	int start_pos = pos % _superblock.block_size();
	int end_pos = pos + size;
	int blocks_affected = (end_pos + _superblock.block_size() - 1) / _superblock.block_size();	
	int written_bytes = 0;

	for(int i = 0; i < blocks_affected; i++) {

		int sp = (start_pos + written_bytes) % _superblock.block_size();
		int ep = 0;

		if (i == blocks_affected - 1) {
			ep = end_pos % _superblock.block_size();
		}
		else {
			ep = _superblock.block_size() - sp;
		}

		std::memcpy(block->data() + sp, content + written_bytes, ep);
		written_bytes += ep;

		save_block(block);

		if (i != blocks_affected - 1) {
			block = AllocateBlock(block->id());
		}
	}

	inode->set_size(written_bytes);
	if (inode->IsDirectoryFlag()) {
		inode->set_size(blocks_affected * _superblock.block_size());
	}

	save_inode(inode);

	delete block;
}
void Storage::WriteByte(INode* inode, int pos, char byte)
{
	Block* block = find_relative_block(inode, pos);

	pos %= _superblock.block_size();
	block->set_char(pos, byte);

	save_block(block);
}
