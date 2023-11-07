#include "file_service.h"

int FileService::get_eof(INode* inode)
{
	return _storage->GetEOF(inode);
}

File* FileService::Create(std::string name, DEntry* parent)
{
	INode* inode = _storage->AllocateInode();
	return new File(inode, parent, name);
}

void FileService::Remove(DEntry* dentry)
{
	_storage->FreeINode(dentry->inode());
}

void FileService::Write(File* file, std::string text)
{
	_storage->ClearBlocks(file->inode());
	_storage->WriteBytes(file->inode(), 0, text.c_str(), text.size());
}

void FileService::Append(File* file, std::string text)
{
	_storage->WriteBytes(file->inode(), get_eof(file->inode()), text.c_str(), text.size());
}

char* FileService::Read(File* file)
{
	return _storage->ReadINodeContent(file->inode());
}
