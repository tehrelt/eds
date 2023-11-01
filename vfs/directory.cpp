#include "directory.h"

bool Directory::_ROOT_EXISTS = false;

Directory::Directory(INode* inode, std::string name)
{
    if (!inode->IsDirectoryFlag() || _ROOT_EXISTS) {
        throw new std::exception();
    }

    _path = Path("");

    _ROOT_EXISTS = true;

    _parent = nullptr;
    _inode = inode;

    _dentries = std::vector<DEntry*>();
    _dentries.push_back(new DEntry(inode, "./"));
}
Directory::Directory(INode* inode, std::string name, INode* parent)
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

Dir Directory::Convert()
{
    Dir dir;

    if (_parent != nullptr) {
        dir.parent_id = _parent->id();
    }
    else {
        dir.parent_id = -1;
    }

    dir.inode_id = _inode->id();
    dir.dentires_count = _dentries.size();
    dir.dentries = new int[_dentries.size()];

    for (int i = 0; i < _dentries.size(); i++) {
        dir.dentries[i] = _dentries[i]->inode_id();
    }

    return dir;
}
