#pragma once
#include <cstdint>
#include <string>
class User
{
private:
	uint_fast32_t _id;
	char _name[16];
	char _hash_password[64];
public:
	User();
	User(char* content);
	User(int id, std::string name, std::string password);

	char* name() { return _name; }


	char* ToChar();
};

