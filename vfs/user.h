#pragma once
#include <cstdint>
#include <string>

constexpr auto USER_RECORD_SIZE = 4 + 16;
constexpr auto FULL_USER_RECORD_SIZE = USER_RECORD_SIZE + 64;

class User
{
private:
	uint_fast32_t _id;
	char _name[16];
public:
	User();
	User(char* content);
	User(int id, std::string name);

	uint_fast32_t id()	{ return _id; }
	char* name()		{ return _name; }


	char* to_char();

};

