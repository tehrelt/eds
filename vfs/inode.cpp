#include "inode.h"

#include <chrono>
#include "tools.h"

void INode::SetSystemFlag() { _flags |= SYSTEM; }
void INode::SetHiddenFlag() { _flags |= HIDDEN; }
void INode::SetArchiveFlag() { _flags |= ARCHIVE; }
void INode::SetDirectoryFlag() { _flags |= DIRECTORY; }

bool INode::IsSystemFlag() { return (_flags & SYSTEM) != 0; }
bool INode::IsHiddenFlag() { return (_flags & HIDDEN) != 0; }
bool INode::IsArchiveFlag() { return (_flags & ARCHIVE) != 0; }
bool INode::IsDirectoryFlag() { return (_flags & DIRECTORY) != 0; }

void INode::UnsetSystemFlag() { _flags &= ~SYSTEM; }
void INode::UnsetHiddenFlag() { _flags &= ~HIDDEN; }
void INode::UnsetArchiveFlag() { _flags &= ~ARCHIVE; }
void INode::UnsetDirectoryFlag() { _flags &= ~DIRECTORY; }

void INode::set_mode(uint_fast8_t mode) { _mode = mode; }
void INode::set_flags(uint_fast8_t flags) { _flags = flags; }

void INode::set_create_date(time_t date) { _create_date = date; }
void INode::set_access_date(time_t date) { _access_date = date; }
void INode::set_modify_date(time_t date) { _modify_date = date; }

void INode::set_block_num(int_fast32_t index) { _block_num = index; }

INode::INode()
{
	_id = 0;
	_flags = 0;
	_mode = 0;
	_uid = 0;
	_block_num = -1;

	_create_date = 0;
	_modify_date = 0;
	_access_date = 0;
}

INode::INode(int id)
{
	_id = id;
	_flags = 0;
	_mode = 0;
	_uid = 0;
	_block_num = -1;

	_create_date = 0;
	_modify_date = 0;
	_access_date = 0;
}

uint_fast8_t INode::flags() { return _flags; }
uint_fast8_t INode::mode() { return _mode; }

uint_fast16_t INode::uid() { return _uid; }

int_fast32_t INode::id() { return _id; }

time_t INode::create_date() { return _create_date; }
time_t INode::modify_date() { return _modify_date; }
time_t INode::access_date() { return _access_date; }

int_fast32_t INode::block_num() { return _block_num; }

std::ostream& operator<<(std::ostream& os, const INode& inode)
{
	os << inode._id << "\t";
	os << std::to_string(inode._flags) << "\t";
	os << std::to_string(inode._mode) << "\t"; 
	os << timeToString(inode._create_date);

	return os;
}
