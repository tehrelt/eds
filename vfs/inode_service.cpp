#include "inode_service.h"

INode* InodeService::Get(int id)
{
	try
	{
		return _storage->GetINode(id);
	}
	catch (const std::exception& e)
	{
		throw e;
	}
}

void InodeService::SetOwner(INode* inode, int uid)
{
	inode->set_uid(uid);
}

void InodeService::SetMode(INode* inode, uint_fast8_t mode)
{
	inode->set_mode(mode);
}

void InodeService::Save(INode* inode)
{
	_storage->SaveINode(inode);
}
