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

void InodeService::Save(INode* inode)
{
	_storage->SaveINode(inode);
}
