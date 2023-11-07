#define _CRT_SECURE_NO_WARNINGS
#include "directory_service.h"

void DirectoryService::save_directory(Directory* directory)
{
	INode* inode = directory->inode();
	int size = directory->CharSize();
	char* content = directory->ToChar();
	_storage->WriteBytes(inode, 0, content, size);
}

Directory* DirectoryService::CreateRoot()
{
	INode* root_inode = _storage->AllocateInode();
	root_inode->SetDirectoryFlag();
	root_inode->SetSystemFlag();
	_storage->SaveINode(root_inode);

	Directory* root = new Directory(root_inode);
	save_directory(root);

	return root;
}
Directory* DirectoryService::ReadRoot()
{
	INode* root_inode = _storage->GetINode(0);
	Block* block = _storage->GetBlock(root_inode->block_num());
	Directory* root = read_directory(root_inode, block->data(), nullptr);
	
	delete block;
	return root;
}

Directory* DirectoryService::Create(std::string name, Directory* parent)
{
	INode* inode = _storage->AllocateInode();
	inode->SetDirectoryFlag();
	_storage->SaveINode(inode);

	Directory* dir = new Directory(inode, parent, name);
	save_directory(dir);

	return dir;
}

void DirectoryService::AddToDirectory(Directory* directory, DEntry* dentry)
{
	directory->add(dentry);
	save_directory(directory);
}

void DirectoryService::RemoveFromDirectory(Directory* d, DEntry* dentry)
{
	d->remove(dentry);
	save_directory(d);
}

std::vector<DEntry*> DirectoryService::GetInfo(Directory* dir)
{
	return dir->dentries();
}

Directory* DirectoryService::Get(INode* inode, Directory* parent)
{
	Block* block = _storage->GetBlock(inode->block_num());
	char* content = block->data();

	return read_directory(inode, content, parent);
}

Directory* DirectoryService::read_directory(INode* inode, char* content, Directory* parent)
{
	int size, entry_inode_id;
	
    char* name = new char[16];

    std::memcpy(&size, content + 16, 4);


	Directory* dir = nullptr;
	if (parent == nullptr) {
		dir = new Directory(inode);
	}
	else {
		char* dir_name = new char[16];
		std::memcpy(dir_name, content, 16);
		dir = new Directory(inode, parent, dir_name);
	}

	dir->clear();

    char* entry = content + 20;
    for (int i = 0; i < size; i++) {

        std::memcpy(&entry_inode_id,	entry,      4);
        std::memcpy(name,				entry + 4, 16);

		INode* entry_inode = _storage->GetINode(entry_inode_id);

        dir->add(new DEntry(entry_inode, dir, name));
        
		entry += DIRECTORY_ENTRY_SIZE;
    }

	return dir;
}
