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

void INode::set_r_____(bool i) 
{ 
	_mode = i ? _mode | R_____ : _mode & ~R_____;
}

void INode::set__w____(bool i)
{
	_mode = i ? _mode | _W____ : _mode & ~_W____;
}

void INode::set___x___(bool i)
{
	_mode = i ? _mode | __X___ : _mode & ~__X___;
}

void INode::set____r__(bool i)
{
	_mode = i ? _mode | ___R__ : _mode & ~___R__;
}

void INode::set_____w_(bool i)
{
	_mode = i ? _mode | ____W_ : _mode & ~____W_;
}

void INode::set______x(bool i)
{
	_mode = i ? _mode | _____X : _mode & ~_____X;
}

bool INode::is_r_____() { return (_mode & R_____) != 0; }
bool INode::is__w____() { return (_mode & _W____) != 0; }
bool INode::is___x___() { return (_mode & __X___) != 0; }

bool INode::is____r__() { return (_mode & ___R__) != 0; }
bool INode::is_____w_() { return (_mode & ____W_) != 0; }
bool INode::is______x() { return (_mode & _____X) != 0; }

void INode::set_mode(uint_fast8_t mode) { _mode = mode; }
void INode::set_flags(uint_fast8_t flags) { _flags = flags; }
void INode::set_uid(uint_fast16_t uid) { _uid = uid; }

void INode::set_create_date(time_t date) { _create_date = date; }
void INode::set_access_date(time_t date) { _access_date = date; }
void INode::set_modify_date(time_t date) { _modify_date = date; }

void INode::set_block_num(int_fast32_t index) { _block_num = index; }
void INode::set_size(uint_fast32_t size) { _size = size; }

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
	_size = 0;
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

	_size = 0;
}

uint_fast8_t INode::flags() { return _flags; }
uint_fast8_t INode::mode() { return _mode; }
uint_fast16_t INode::uid() { return _uid; }
int_fast32_t INode::id() { return _id; }

time_t INode::create_date() { return _create_date; }
time_t INode::modify_date() { return _modify_date; }
time_t INode::access_date() { return _access_date; }

int_fast32_t INode::block_num() { return _block_num; }
uint_fast32_t INode::size() { return _size; }

std::ostream& operator<<(std::ostream& os, const INode& inode)
{
	char flags[5];
	flags[4] = '\0';

	for (int i = 0; i < 4; i++) { flags[i] = '-'; }
	

	if ((inode._flags & SYSTEM) != 0)		{ flags[0] = 's'; } 
	if ((inode._flags & HIDDEN) != 0)		{ flags[1] = 'h'; }
	if ((inode._flags & ARCHIVE) != 0)		{ flags[2] = 'a'; }
	if ((inode._flags & DIRECTORY) != 0)	{ flags[3] = 'd'; }

	char mode[7];
	mode[6] = '\0';

	for (int i = 0; i < 6; i++) {
		mode[i] = '-';
	}

	if ((inode._mode & R_____) != 0) { mode[0] = 'r'; }
	if ((inode._mode & _W____) != 0) { mode[1] = 'w'; }
	if ((inode._mode & __X___) != 0) { mode[2] = 'x'; }
	if ((inode._mode & ___R__) != 0) { mode[3] = 'r'; }
	if ((inode._mode & ____W_) != 0) { mode[4] = 'w'; }
	if ((inode._mode & _____X) != 0) { mode[5] = 'x'; }

	os << inode._id << "\t";
	os << flags << "\t";
	os << mode << "\t";
	os << inode._size << "\t";
	os << timeToString(inode._create_date) << "\t";
	os << timeToString(inode._modify_date) << "\t";
	os << timeToString(inode._access_date) << "\t";
	os << inode._block_num;

	return os;
}
