#pragma once
#include <string>
#include "service.h"
#include "user.h"

class FileSystem
{
	Service* _services;

	Directory* _root_directory;
	Directory* _current_directory;

	User* _root_user;
	User* _current_user;

public:
	FileSystem();
	FileSystem(Service* services);

	Service*	services()				{ return _services;			  }
	Directory*  root_directory()		{ return _root_directory;	  }
	Directory*	current_directory()		{ return _current_directory;  }
	User*		current_user()			{ return _current_user;		  }

	void ChangeDirectory(Directory* dir);
	void ChangeToRootDirectory();

	char* GetBlockContent(int inode_id);

	Directory* GetDirectory(int inode_id);
	Directory* GetParentDirectory();
	Directory* GetParentDirectory(Directory* at);

	Block* GetBlock(int id);
	INode* GetInode(int id);

	File* CreateFile(std::string name);
	File* CreateFileAt(std::string name, Directory* dir);
	void RemoveFile(int inode_id);


	Directory* CreateDirectory(std::string name);
	Directory* CreateDirectoryAt(std::string name, Directory* directory);


	User* CreateUser(std::string name, std::string password);
	User* GetUser(int id);


	void AppendFile(int inode_id, std::string text);
	void WriteFile(int inode_id, std::string text);
	char* ReadFile(int inode_id);

	std::vector<DEntry*> ls();
	Superblock* sb();

	static FileSystem* Create(std::string name, uint_fast64_t size);
	static FileSystem* Mount (std::string name);
};



