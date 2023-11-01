#pragma once
#include <string>
class Repository
{
private:
	std::string _file_name;

protected:
	uint_fast32_t _location;
	uint_fast32_t _record_size;

	void write(char* source, int offset, int size);
	void read(char* destination, int offset, int size);

public:
	Repository();
	Repository(std::string file_name, uint_fast32_t location, uint_fast32_t record_size);
};

