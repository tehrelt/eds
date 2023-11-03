#include "user.h"
#include "sha256.h"

User::User()
{
	_id = 0;
	strncpy(_name, "", 16);
	strncpy(_hash_password, "", 256);
}

User::User(int id, std::string name, std::string password)
{
	_id = id;
	strncpy(_name, name.c_str(), 16);
	strncpy(_hash_password, sha256(password).c_str(), 256);
}
