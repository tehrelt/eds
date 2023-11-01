#include "service.h"

Service::Service()
{
	_file_service = FileService();
	_directory_service = DirectoryService();
}
Service::Service(Storage* storage)
{
	_file_service = FileService(storage);
	_directory_service = DirectoryService(storage);
	_block_service = BlockService(storage);
	_inode_service = InodeService(storage);
}