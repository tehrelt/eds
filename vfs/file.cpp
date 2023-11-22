#include "file.h"

File::File(INode* inode, DEntry* parent, const std::string& name)
	: DEntry(inode, parent, name)
{
	_position = 0;
}

void File::write(const char* bytes, int length)
{
	Storage::STORAGE()->writeBytes(_inode, _position, bytes, length);
}
//void File::write(const char* bytes, int offset, int length)
//{
//	Storage::STORAGE()->writeBytes(_inode, offset, bytes, length);
//}

char* File::read()
{
	return  Storage::STORAGE()->readBytes(_inode);
}
char* File::read(int length)
{
	throw std::exception("File::read: not implemented");
}

void File::seek(int offset)
{
	_position += offset;
}
void File::seek(int offset, int start)
{
	_position = start + offset;
}

int File::tell()
{
	return _position;
}

int File::length()		  { return _inode->size();		  }

time_t File::creationAt() { return _inode->create_date(); }
time_t File::modifiedAt() { return _inode->modify_date(); }
time_t File::accessedAt() { return _inode->access_date(); }

bool File::eof() { return _position >= _inode->size(); }
