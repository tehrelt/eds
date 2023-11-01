#include "inode_service.h"

Inode* InodeService::Get(int id)
{
    return _storage->GetInode(id);
}
