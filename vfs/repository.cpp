#include "repository.h"

#include <fstream>

void Repository::write(char* source, int offset, int size)
{
    std::fstream stream;

    stream.open(_file_name, std::ios::binary | std::ios::out | std::ios::in);

    if (!stream.is_open())
        throw new std::exception();


    stream.seekp(offset, std::ios::beg);
    stream.write(source, size);

    stream.close();
}
void Repository::read(char* destination, int offset, int size)
{
    std::ifstream stream;

    stream.open(_file_name, std::ios::binary | std::ios::in);

    if (!stream.is_open())
        throw new std::exception();

    stream.seekg(offset, std::ios::beg);
    stream.read(destination, size);

    stream.close();
}

Repository::Repository(std::string file_name)
{
	_file_name = file_name;
}
