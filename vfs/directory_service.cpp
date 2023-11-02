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
	root_inode->SetSystemFlag();

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

Directory* DirectoryService::Create(std::string name, Directory* parent)
{
	INode* inode = _storage->AllocateInode();

	inode->SetDirectoryFlag();

	_storage->SaveINode(inode);

	INode* parent_inode = _storage->GetINode(parent->inode_id());

	Directory* dir = new Directory(inode, name, parent_inode);

	save_directory(dir);

	return dir;
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

Directory* DirectoryService::Get(int inode_id)
{
	INode* inode = _storage->GetINode(inode_id);
	Block* block = _storage->GetBlock(inode->block_num());
	char* content = block->data();

	delete inode;
	return new Directory(content);
}
