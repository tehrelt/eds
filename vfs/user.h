#pragma once
#include <cstdint>
#include <string>
class User
{
private:
	uint_fast32_t _id;
	char _name[16];
	char _hash_password[256];
public:
	User();
	User(int id, std::string name, std::string password);
};

