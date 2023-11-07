#define _CRT_SECURE_NO_WARNINGS
#include "user.h"
#include "sha256.h"
#include "storage.h"
#include "directory.h"

User::User()
{
	_id = 0;
	strncpy(_name, "", 16);
}

User::User(char* content)
{
	std::memcpy(&_id,			content,		  4);
	std::memcpy(_name,			content + 4,	  16);
}

User::User(int id, std::string name)
{
	_id = id;
	strncpy(_name,			name.c_str(),			16);
}

char* User::to_char()
{
	char* content = new char[USER_RECORD_SIZE];

	std::memcpy(content, &_id, 4);
	std::memcpy(content + 4, _name, 16);

	return content;
}

