#include "file.h"

File::File(INode* inode, DEntry* parent, const std::string& name)
	: DEntry(inode, parent, name)
{
	position = 0;
}

void File::write(const char* bytes, int length)
{
	Storage::STORAGE()->writeBytes(_inode, 0, bytes, length);
}

void File::write(const char* bytes, int offset, int length)
{
	Storage::STORAGE()->writeBytes(_inode, offset, bytes, length);
}

char* File::read()
{
	return  Storage::STORAGE()->readBytes(_inode);
}

char* File::read(int length)
{
	throw std::exception("File::read: not implemented");
}

char* File::read(int offset, int length)
{
	throw std::exception("File::read: not implemented");
}

int File::length()		  { return _inode->size();		  }
time_t File::creationAt() { return _inode->create_date(); }
time_t File::modifiedAt() { return _inode->modify_date(); }
time_t File::accessedAt() { return _inode->access_date(); }

bool File::eof() { return position >= _inode->size(); }
