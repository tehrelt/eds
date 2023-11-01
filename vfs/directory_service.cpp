#define _CRT_SECURE_NO_WARNINGS
#include "directory_service.h"

Directory* DirectoryService::CreateRoot()
{
	INode* root_inode = _storage->AllocateInode();

	Directory* root = new Directory(root_inode, "");

	Dir d_root = root->Convert();

	/*int size = sizeof(int) * 4 + sizeof(int) * d_root.dentires_count;
	char* content = new char[size];*/
	char* content = new char[0];

	strcat(content, (char*)&d_root.parent_id);
	strcat(content, (char*)&d_root.inode_id);
	strcat(content, (char*)&d_root.dentires_count);
	for (int i = 0; i < d_root.dentires_count; i++) {
		strcat(content, (char*)&d_root.dentries[i]);
	}

	_storage->WriteBytes(root_inode, 0, content, strlen(content));

	return root;
}
