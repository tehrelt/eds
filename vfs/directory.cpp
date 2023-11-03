#define _CRT_SECURE_NO_WARNINGS
#include "directory.h"
#include "tools.h"

bool Directory::_ROOT_EXISTS = false;


Directory::Directory(INode* inode, std::string name)
{
    if (!inode->IsDirectoryFlag() || _ROOT_EXISTS) {
        throw new std::exception();
    }

    _path = Path("");

    _ROOT_EXISTS = true;

    _parent_id = -1;
    _inode_id = inode->id();

    _dentries = std::vector<DEntry*>();
    _dentries.push_back(new DEntry(inode, "./"));
}
Directory::Directory(char* content)
{
    int size, idid;
    char* name = new char[16];
    int entry_size = 4 + 16;

    std::memcpy(&_parent_id, content,     4);   
    std::memcpy(&_inode_id,  content + 4, 4);
    std::memcpy(&size,       content + 8, 4);

    _dentries = std::vector<DEntry*>();

    char* entry = content + 12;
    for (int i = 0; i < size; i++) {
        std::memcpy(&idid, entry,     4);
        std::memcpy(name,  entry + 4, 16);

        _dentries.push_back(new DEntry(idid, name));
        entry += entry_size;
    }
}
Directory::Directory(INode* inode, std::string name, INode* parent)
{
    if (!inode->IsDirectoryFlag() && _ROOT_EXISTS) {
        throw new std::exception();
    }

    _path;

    _parent_id = parent->id();
    _inode_id = inode->id();

    _dentries = std::vector<DEntry*>();
    _dentries.push_back(new DEntry(inode,  "./"));
    _dentries.push_back(new DEntry(parent, "../"));
}

void Directory::add(DEntry* dentry)
{
    _dentries.push_back(dentry);
}

void Directory::remove(int dentry_idx)
{
    _dentries.erase(std::next(_dentries.begin(), dentry_idx));
}

char* Directory::ConvertToChar()
{
    int entry_size = 4 + 16;
    char* c = new char[3 * 4 + entry_size * _dentries.size()];


    std::memcpy(c,     &_parent_id, sizeof(int));
    std::memcpy(c + 4, &_inode_id,  sizeof(int));

    int size = _dentries.size();
    std::memcpy(c + 8, &size, sizeof(int));

    char* current_entry = c + 12;

    for (int i = 0; i < _dentries.size(); i++) {
        int idid = _dentries[i]->inode_id();
        std::memcpy(current_entry, &idid, sizeof(int));
        std::memcpy(current_entry + 4, _dentries[i]->name(), 16);

        current_entry += entry_size;
    }

    return c;
}

int Directory::CharSize()
{
    return 3*sizeof(int) + (sizeof(int) + 16) * _dentries.size();
}

//Dir Directory::Convert()
//{
//    Dir dir;
//
//    if (_parent != nullptr) {
//        dir.parent_id = _parent->id();
//    }
//    else {
//        dir.parent_id = -1;
//    }
//
//    dir.inode_id = _inode->id();
//    dir.dentires_count = _dentries.size();
//    dir.dentries = new DEntry[_dentries.size()];
//
//    for (int i = 0; i < _dentries.size(); i++) {
//        dir.dentries[i] = _dentries[i];
//    }
//
//    return dir;
//}
