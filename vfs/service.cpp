#include "service.h"

Service::Service()
{
	_file_service = FileService();
	_directory_service = DirectoryService();
	_block_service = BlockService();
	_inode_service = InodeService();
}
Service::Service(Storage* storage)
{
	_file_service = FileService(storage);
	_directory_service = DirectoryService(storage);
	_block_service = BlockService(storage);
	_inode_service = InodeService(storage);
	_user_service = UserService(storage);
}