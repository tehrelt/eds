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

	static FileSystem* _INSTANCE;

public:

	Directory*  root_directory()		{ return _root_directory;	  }
	Directory*	current_directory()		{ return _current_directory;  }
	User*		current_user()			{ return _current_user;		  }

	void init();

	User* CreateUser(const std::string& username, const std::string& pass);
	User* getUserById(int id);
	User* findUserByName(const std::string& username);
	bool Login(const std::string& username, const std::string& pass);

	Superblock* sb();

	static void Create(std::string name, uint_fast64_t size);
	static void Mount (std::string name);
	static FileSystem* FILE_SYSTEM() { return _INSTANCE; }
};