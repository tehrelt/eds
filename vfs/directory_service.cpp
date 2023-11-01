#define _CRT_SECURE_NO_WARNINGS
#include "directory_service.h"

Directory* DirectoryService::CreateRoot()
{
	INode* root_inode = _storage->AllocateInode();

	root_inode->SetDirectoryFlag();

	_storage->SaveINode(root_inode);

	Directory* root = new Directory(root_inode, "");

	Dir dr = root->Convert();

	char* content = new char[0];

	strcpy(content, (char*)&dr.parent_id);
	strcat(content, (char*)&dr.inode_id);
	strcat(content, (char*)&dr.dentires_count);
	for (int i = 0; i < dr.dentires_count; i++) {
		strcat(content, (char*)&dr.dentries[i]);
		strcat(content, (char*)root->dentry()[i]->name());
	}


	int size = 3 * sizeof(int) +  dr.dentires_count * (sizeof(int) + 16);
	_storage->WriteBytes(root_inode, 0, content, size);

	return root;
}
Directory* DirectoryService::ReadRoot()
{
	INode* root = _storage->GetINode(0);

	Block* block = _storage->GetBlock(root->block_num());

	char* block_content = block->data();

	Dir dr;

	strcpy((char*)&dr.parent_id, block_content);

	return nullptr;
}

std::vector<DEntry*> DirectoryService::GetInfo(Directory* dir)
{
	return dir->dentry();
}
