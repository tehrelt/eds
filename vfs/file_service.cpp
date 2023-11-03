#include "file_service.h"

File* FileService::Create(std::string name)
{
	INode* inode = _storage->AllocateInode();

	return new File(inode);
}

char* FileService::Read(int inode_id)
{
	return _storage->ReadINodeContent(inode_id);
}
