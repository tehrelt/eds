#pragma once
#include "file_service.h"
#include "directory_service.h"
#include "storage.h"
#include "block_service.h"
#include "inode_service.h"

class Service
{
	FileService _file_service;
	DirectoryService _directory_service;
	InodeService _inode_service;
	BlockService _block_service;

public:
	Service();
	Service(Storage* storage);

	FileService* file_service() { return &_file_service; }
	DirectoryService* directory_service() { return &_directory_service; }
	InodeService* inode_service() { return &_inode_service; }
	BlockService* block_service() { return &_block_service; }
};

