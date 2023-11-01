#include "inode.h"

#include <chrono>
#include "tools.h"

void Inode::SetSystemFlag() { _flags |= SYSTEM; }
void Inode::SetHiddenFlag() { _flags |= HIDDEN; }
void Inode::SetArchiveFlag() { _flags |= ARCHIVE; }
void Inode::SetDirectoryFlag() { _flags |= DIRECTORY; }

bool Inode::IsSystemFlag() { return (_flags & SYSTEM) != 0; }
bool Inode::IsHiddenFlag() { return (_flags & HIDDEN) != 0; }
bool Inode::IsArchiveFlag() { return (_flags & ARCHIVE) != 0; }
bool Inode::IsDirectoryFlag() { return (_flags & DIRECTORY) != 0; }

void Inode::UnsetSystemFlag() { _flags &= ~SYSTEM; }
void Inode::UnsetHiddenFlag() { _flags &= ~HIDDEN; }
void Inode::UnsetArchiveFlag() { _flags &= ~ARCHIVE; }
void Inode::UnsetDirectoryFlag() { _flags &= ~DIRECTORY; }

void Inode::set_mode(uint_fast8_t mode) { _mode = mode; }
void Inode::set_flags(uint_fast8_t flags) { _flags = flags; }

void Inode::set_create_date(time_t date) { _create_date = date; }
void Inode::set_access_date(time_t date) { _access_date = date; }
void Inode::set_modify_date(time_t date) { _modify_date = date; }

void Inode::set_block_num(int_fast32_t index) { _block_num = index; }

Inode::Inode()
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

Inode::Inode(int id)
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

uint_fast8_t Inode::flags() { return _flags; }
uint_fast8_t Inode::mode() { return _mode; }

uint_fast16_t Inode::uid() { return _uid; }

int_fast32_t Inode::id() { return _id; }

time_t Inode::create_date() { return _create_date; }
time_t Inode::modify_date() { return _modify_date; }
time_t Inode::access_date() { return _access_date; }

int_fast32_t Inode::block_num() { return _block_num; }

std::ostream& operator<<(std::ostream& os, const Inode& inode)
{
	os << inode._id << "\t";
	os << std::to_string(inode._flags) << "\t";
	os << std::to_string(inode._mode) << "\t"; 
	os << timeToString(inode._create_date);

	return os;
}
