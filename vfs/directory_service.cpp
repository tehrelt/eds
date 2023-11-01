#define _CRT_SECURE_NO_WARNINGS
#include "directory_service.h"

//Directory* DirectoryService::CreateRoot()
//{
//	Block* block = _file_system->storage()->block_repository()->Allocate();
//	Inode* inode = _file_system->storage()->inode_repository()->Allocate();
//
//	inode->set_create_date(getCurrentDate());
//	inode->set_block_num(block->id());
//
//	Directory* dir = new Directory(inode, "");
//
//	int size = 4 * 3 + dir->dentry().size() * (4 + 16);
//
//	char* content = block->data();
//	char* dir_content = new char[0];
//
//	uint_fast32_t inode_id = dir->inode()->id();
//	uint_fast32_t dentry_count = dir->dentry().size();
//
//	strcat(dir_content, "rrrr");
//	strcat(dir_content, (char*)&inode_id);
//	strcat(dir_content, (char*)&dentry_count);
//
//	for (int i = 0; i < dir->dentry().size(); i++) {
//
//		uint_fast32_t child_inode_id = dir->dentry()[i]->inode_id();
//		char* child_name = new char[16];
//		char* raw_name = dir->dentry()[i]->name();
//
//		strncpy(child_name, raw_name, 16);
//
//		strcat(dir_content, (char*)&child_inode_id);
//		strcat(dir_content, child_name);
//	}
//
//	strncpy(content, dir_content, size);
//	
//	_file_system->storage()->inode_repository()->Save(inode);
//	_file_system->storage()->block_repository()->Save(block);
//
//	return dir;
//}
//
//Directory* DirectoryService::Get(Directory* current)
//{
//	for (int i = 0; i < current->dentry().size(); i++) {
//		Inode* inode = current->dentry()[i]->inode();
//		std::cout << *inode << std::endl;
//	}
//	return nullptr;
//}
