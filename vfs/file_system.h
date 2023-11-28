#pragma once
#include <string>
#include "user.h"
#include "directory.h"

class FileSystem
{
	Directory* _root_directory;
	Directory* _current_directory;

	User* _root_user;
	User* _current_user;

public:
	Directory*  root_directory()		{ return _root_directory;	  }
	Directory*	current_directory()		{ return _current_directory;  }
	User*		current_user()			{ return _current_user;		  }

	void init();

	Directory* forwardTo(Directory* to);

	User* createUser(const std::string& username, const std::string& pass);
	User* getUserById(int id);
	bool userExists(const std::string& username);
	User* findUserByName(const std::string& username);
	bool login(const std::string& username, const std::string& pass);

	bool checkOwner(INode* inode);

	Superblock* sb();

	static FileSystem* Create(std::string name, uint_fast64_t size);
	static FileSystem* Mount (std::string name);
};