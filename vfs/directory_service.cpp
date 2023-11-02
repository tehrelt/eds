#define _CRT_SECURE_NO_WARNINGS
#include "directory_service.h"

Directory* DirectoryService::CreateRoot()
{
	INode* root_inode = _storage->AllocateInode();

	root_inode->SetDirectoryFlag();

	_storage->SaveINode(root_inode);

	Directory* root = new Directory(root_inode, "");

	int size = root->CharSize();
	char* content = root->ConvertToChar();
	
	_storage->WriteBytes(root_inode, 0, content, size);

	return root;
}
Directory* DirectoryService::ReadRoot()
{
	INode* root_inode = _storage->GetINode(0);

	Block* block = _storage->GetBlock(root_inode->block_num());

	char* block_content = block->data();

	Directory* root = new Directory(block_content);

	return root;
}

std::vector<DEntry*> DirectoryService::GetInfo(Directory* dir)
{
	return dir->dentry();
}
