#include "file_service.h"

int FileService::get_eof(INode* inode)
{
	return _storage->GetEOF(inode);
}

File* FileService::Create(std::string name)
{
	INode* inode = _storage->AllocateInode();

	return new File(inode);
}

void FileService::Remove(int inode_id)
{
	_storage->FreeINode(inode_id);
}

void FileService::Remove(INode* inode)
{
	_storage->FreeINode(inode);
}

void FileService::Write(INode* inode, std::string text)
{
	_storage->ClearBlocks(inode);
	_storage->WriteBytes(inode, 0, text.c_str(), text.size());
}

void FileService::Append(INode* inode, std::string text)
{
	_storage->WriteBytes(inode, get_eof(inode), text.c_str(), text.size());
}

char* FileService::Read(int inode_id)
{
	return _storage->ReadINodeContent(inode_id);
}
