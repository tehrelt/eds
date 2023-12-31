#include "storage.h"
#include <fstream>
#include "tools.h"

Storage* Storage::_INSTANCE = nullptr;

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

	Log log("Storage::read_inode");
	log.debug("reading inode#" + std::to_string(id));

	return inode;
}

INode* Storage::find_free_inode()
{
	for (int i = 0; i < _imap.capacity(); i++) {
		if (!_imap.IsLocked(i)) {
			Log log("Storage::find_free_inode");
			log.debug("found free inode#" + std::to_string(i));
			return &_imap[i];
		}
	}
	return nullptr;
}
Block* Storage::find_free_block()
{
	for (int i = 0; i < _fat.capacity(); i++) {
		if (_fat[i] == -1) {
			Log log("Storage::find_free_block");
			log.debug("found free block#" + std::to_string(i));
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
	Log log("Storage::save_superblock");
	write((char*)&_superblock, 0, sizeof(Superblock));
	log.debug("superblock saved");
}
void Storage::save_block(Block* block)
{
	char* content = block->data();
	write(content, block->id() * _superblock.block_size(), _superblock.block_size());
	Log log("Storage::save_block");
	log.debug("saved block#" + std::to_string(block->id()));
}
void Storage::saveINode(INode* inode)
{
	write((char*)inode, 
		_superblock.num_of_first_imap_block() * _superblock.block_size() + inode->id() * sizeof(INode),
		sizeof(INode));
	Log log("Storage::saveINode");
	log.debug("saved inode#" + std::to_string(inode->id()));
}

void Storage::unlock_inode(INode* inode)
{
	_imap.Unlock(inode->id());
	auto part = _imap.part(inode->id());
	auto part_idx = _imap.get_part_index(inode->id());
	write((char*)&part,
		_superblock.num_of_first_part_block() * _superblock.block_size() + part_idx * sizeof(part),
		sizeof(part));
	Log log("Storage::unlock_inode");
	log.debug("unlocked inode#" + std::to_string(inode->id()));
}
void Storage::unlock_inode(int inode_id)
{
	_imap.Unlock(inode_id);
	auto part = _imap.part(inode_id);
	auto part_idx = _imap.get_part_index(inode_id);
	write((char*)&part,
		_superblock.num_of_first_part_block() * _superblock.block_size() + part_idx * sizeof(part),
		sizeof(part));
	Log log("Storage::unlock_inode");
	log.debug("unlocked inode#" + std::to_string(inode_id));
}
void Storage::lock_inode(INode* inode)
{
	_imap.Lock(inode->id());
	auto part = _imap.part(inode->id());
	auto part_idx = _imap.get_part_index(inode->id());
	write((char*)&part,
		_superblock.num_of_first_part_block() * _superblock.block_size() + part_idx * sizeof(part),
		sizeof(part));
	Log log("Storage::lock_inode");
	log.debug("locked inode#" + std::to_string(inode->id()));
}
void Storage::lock_inode(int inode_id)
{
	_imap.Lock(inode_id);
	auto part = _imap.part(inode_id);
	auto part_idx = _imap.get_part_index(inode_id);
	write((char*)&part,
		_superblock.num_of_first_part_block() * _superblock.block_size() + part_idx * sizeof(part),
		sizeof(part));
	Log log("Storage::lock_inode");
	log.debug("locked inode#" + std::to_string(inode_id));
}

void Storage::free_block(int num)
{
	set_fat_record(num, -1);
	_superblock += _superblock.block_size();
	Log log("Storage::free_block");
	log.debug("block#" + std::to_string(num) + " is now free");
}

void Storage::set_fat_record(int idx, int value)
{
	Log log("Storage::set_fat_record");
	_fat[idx] = value;
	log.debug("fat[" + std::to_string(idx) + "] = " + std::to_string(value));
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

	if (!stream.is_open()) {
		throw new std::exception();
	}

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

int Storage::getNextUID()
{
	return _superblock.users_count();
}

void Storage::addUser()
{
	_superblock.add_user();
	save_superblock();
}

INode* Storage::allocateINode()
{
	Log log("Storage::allocateINode");
	INode* inode = find_free_inode();

	if (inode == nullptr) {
		log.info("Free inodes is out");
		throw std::exception("cannot allocate inode");
	}

	Block* free_block = allocateBlock();

	auto current_datetime = getCurrentDate();
	inode->set_flags(0);
	inode->set_uid(0);
	inode->set_mode(0b110100);
	inode->set_create_date(current_datetime);
	inode->set_modify_date(current_datetime);
	inode->set_access_date(current_datetime);
	inode->set_size(0);

	inode->set_block_num(free_block->id());

	lock_inode(inode);

	saveINode(inode);

	return inode;
}
Block* Storage::allocateBlock()
{
	Log log("Storage::allocateBlock");
	Block* block = find_free_block();
	if (block == nullptr) {
		log.warn("free space is out");
		throw std::exception("not enough space");
	}
	block->set_char(0, '\0');
	save_block(block);

	_superblock -= _superblock.block_size();

	set_fat_record(block->id(), -2);

	save_superblock();

	return block;
}
Block* Storage::allocateBlock(int prev_id)
{
	Log log("Storage::allocateBlock");
	Block* block = find_free_block();

	if (block == nullptr) {
		log.warn("free space is out");
		throw std::exception("not enough space");
	}

	save_block(block);

	_superblock -= _superblock.block_size();

	set_fat_record(prev_id, block->id());
	set_fat_record(block->id(), -2);

	save_superblock();

	return block;
}

void Storage::freeINode(INode* inode)
{
	Log log("Storage::freeINode");
	unlock_inode(inode);

	int bn = inode->block_num();

	auto blockchain = getBlockchain(bn);
	std::stringstream ss;
	for (auto& i : blockchain)
	{
		ss << i << " ";
	}

	log.debug("inode#" + std::to_string(inode->id()) + " has blockchain - " + ss.str());

	while (bn != -2) {
		int t = bn;
		bn = _fat[bn];
		free_block(t);
	}
}
void Storage::clearBlocks(INode* inode)
{
	Block* clear_block = new Block(0, _superblock.block_size());
	Block* block = nullptr;
	
	int id = inode->block_num();
	while (id != -2) {
		block = getBlock(id);
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

std::vector<int> Storage::getBlockchain(int block_id)
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

Block* Storage::getBlock(int id)
{
	if (id < _superblock.num_of_first_data_block()) {
		throw new std::exception("������ ����������� ����� ����������������� ��� �������");
	}
	else if (id >= _superblock.fat_capacity()) {
		throw new std::exception("�������� id ������ �� ������� FAT");
	}

	return read_block(id);
}

INode* Storage::getINode(int id)
{
	if (id >= _superblock.imap_capacity()) {
		throw std::exception("�������� id ������ �� ������� IMap");
	}
	return read_inode(id);
}

char* Storage::readBytes(INode* inode)
{
	int total_bytes = inode->size() == 0 ? 1 : inode->size();
	char* content = new char[total_bytes];
	content[total_bytes - 1] = '\0';

	int p = 0;
	int id = inode->block_num();

	while (total_bytes > 0) {
		int to_read = total_bytes > _superblock.block_size()
								? _superblock.block_size()
								: total_bytes;

		
		char* bc = new char[to_read];

		read(bc, id * _superblock.block_size(), to_read);
		std::memcpy(content + p, bc, to_read);

		total_bytes -= to_read;
		p += to_read;
		id = _fat[id];

		delete[] bc;
	}

	inode->set_access_date(getCurrentDate());
	saveINode(inode);

	return content;
}

void Storage::writeBytes(INode* inode, int pos, const char* content, int size)
{
	Log log("Storage::writeBytes");
	Block* block = find_relative_block(inode, pos);
	
	int start_pos = pos % _superblock.block_size();
	int end_pos = pos + size;
	int blocks_affected = (end_pos + _superblock.block_size() - 1) / _superblock.block_size();	
	int block_num = pos  / _superblock.block_size();
	int written_bytes = 0;

	
	for(int i = block_num; i < blocks_affected; i++) {

		int sp = 0;
		int ep = _superblock.block_size();

		if (i == block_num) {
			sp = start_pos;
		}
		if (i == blocks_affected - 1 && end_pos != _superblock.block_size()) {
			ep = end_pos % _superblock.block_size();
		}

		std::memcpy(block->data() + sp, content + written_bytes, ep - sp);
		written_bytes += ep - sp;

		save_block(block);

		if (i != blocks_affected - 1) {
			try
			{
				block = allocateBlock(block->id());
			}
			catch (const std::exception&)
			{
				break;
			}
			
		}
	}

	inode->set_size(pos + written_bytes);
	if (inode->IsDirectoryFlag()) {
		inode->set_size(blocks_affected * _superblock.block_size());
	}

	auto current_datetime = getCurrentDate();
	inode->set_modify_date(current_datetime);
	inode->set_access_date(current_datetime);
	saveINode(inode);

	log.debug("written a " + std::to_string(written_bytes) + " to inode#" + std::to_string(inode->id()));

	delete block;
}

int Storage::GetEOF(INode* inode)
{
	auto chain = getBlockchain(inode->block_num());

	int block_offset = chain.size() - 2;
	int last_block = chain.at(block_offset);

	char* content = getBlock(last_block)->data();
	int bytes_offset = block_offset * _superblock.block_size() + strlen(content);
	return bytes_offset;
}

void Storage::INIT_STORAGE(std::string name, Superblock* sb, FAT* fat, IMap* imap)
{
	_INSTANCE = new Storage(name, sb, fat, imap);
}

Storage* Storage::INSTANCE()
{
	return _INSTANCE;
}
