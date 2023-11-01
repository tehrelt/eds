#include "inode_service.h"

INode* InodeService::Get(int id)
{
	try
	{
		return _storage->GetInode(id);
	}
	catch (const std::exception& e)
	{
		throw e;
	}
}
