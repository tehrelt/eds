#define _CRT_SECURE_NO_WARNINGS
#include "directory_service.h"

void DirectoryService::save_directory(Directory* directory)
{
	INode* inode = _storage->GetINode(directory->inode_id());
	int size = directory->CharSize();
	char* content = directory->ConvertToChar();
	_storage->WriteBytes(inode, 0, content, size);
}

Directory* DirectoryService::read_directory()
{
	return nullptr;
}

Directory* DirectoryService::CreateRoot()
{
	INode* root_inode = _storage->AllocateInode();

	root_inode->SetDirectoryFlag();

	_storage->SaveINode(root_inode);

	Directory* root = new Directory(root_inode, "");

	save_directory(root);

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

Directory* DirectoryService::AddToDirectory(Directory* directory, DEntry* dentry)
{
	directory->add(dentry);
	
	save_directory(directory);

	return directory;
}

std::vector<DEntry*> DirectoryService::GetInfo(Directory* dir)
{
	return dir->dentry();
}
