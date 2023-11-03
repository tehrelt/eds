#define _CRT_SECURE_NO_WARNINGS
#include "user.h"
#include "sha256.h"

User::User()
{
	_id = 0;
	strncpy(_name, "", 16);
	strncpy(_hash_password, "", 256);
}

User::User(char* content)
{
	std::memcpy(&_id,			content,		  4);
	std::memcpy(_name,			content + 4,	  16);
	std::memcpy(_hash_password, content + 4 + 16, 256);
}

User::User(int id, std::string name, std::string hash_password)
{
	_id = id;
	strncpy(_name,			name.c_str(),			16);
	strncpy(_hash_password, hash_password.c_str(),	256);
}

char* User::ToChar()
{
	int size = 4 + 16 + 256;

	char* content = new char[size];

	std::memcpy(content, &_id, 4);
	std::memcpy(content + 4, _name, 16);
	std::memcpy(content + 4 + 16, _hash_password, 256);

	return content;
}
