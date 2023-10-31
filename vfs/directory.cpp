#include "directory.h"

Directory::Directory(Inode* inode)
{
    if (!inode->IsDirectoryFlag()) {
        throw new std::exception();
    }

    _inode = inode;
}

void Directory::CreateDirectory(std::string name)
{
}

void Directory::CreateFile(std::string name)
{
}

void Directory::Start()
{
}

bool Directory::Exists(std::string name)
{
    return false;
}

File Directory::Open(std::string name)
{
    return File();
}

void Directory::Move(std::string source, std::string destination)
{
}

void Directory::RemoveFile()
{
}

DEntry Directory::First()
{
    return DEntry();
}
