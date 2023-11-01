#include "directory.h"

bool Directory::_ROOT_EXISTS = false;

Directory::Directory(Inode* inode, std::string name)
{
    if (!inode->IsDirectoryFlag() && _ROOT_EXISTS) {
        throw new std::exception();
    }

    _ROOT_EXISTS = true;

    _parent = nullptr;
    _inode = inode;

    _dentries = std::vector<DEntry*>();
    _dentries.push_back(new DEntry(inode, "./"));
}

Directory::Directory(Inode* inode, std::string name, Inode* parent)
{
    if (!inode->IsDirectoryFlag() && _ROOT_EXISTS) {
        throw new std::exception();
    }

    _parent = parent;
    _inode = inode;

    _dentries = std::vector<DEntry*>();
    _dentries.push_back(new DEntry(inode,  "./"));
    _dentries.push_back(new DEntry(parent, "../"));
}