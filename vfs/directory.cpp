#define _CRT_SECURE_NO_WARNINGS
#include "directory.h"
#include "tools.h"

Directory::Directory(INode* inode) 
    : DEntry(inode, nullptr, "")
{
    _dentries = std::vector<DEntry*>();
    _dentries.push_back(new DEntry(inode, this, "./"));
}

Directory::Directory(INode* inode, DEntry* parent, const std::string& name)
    : DEntry(inode, parent, name)
{
    if (!inode->IsDirectoryFlag()) {
        throw new std::exception();
    }
    _dentries = std::vector<DEntry*>();
    _dentries.push_back(new DEntry(inode, this,  "./"));
    _dentries.push_back(new DEntry(parent->inode(), this, "../"));
}

DEntry* Directory::exists(const std::string& name)
{
    for (auto dentry : _dentries) {
        if (name == dentry->name()) {
            return dentry;
        }
    }
    return nullptr;
}

void Directory::add(DEntry* dentry)
{
    _dentries.push_back(dentry);
}

void Directory::remove(int dentry_idx)
{
    _dentries.erase(std::next(_dentries.begin(), dentry_idx));
}
void Directory::remove(DEntry* dentry)
{
    auto it = std::find(_dentries.begin(), _dentries.end(), dentry);
    _dentries.erase(it);
}

void Directory::clear()
{
    _dentries.clear();
}

char* Directory::ToChar()
{
    char* c = new char[DIRECTORY_RECORD_CHAR_SIZE];

    int inode_id = _inode->id();

    std::memcpy(c, _name, 16);
    int size = _dentries.size();
    std::memcpy(c + 16, &size, sizeof(int));

    char* current_entry = c + 20;

    for (int i = 0; i < _dentries.size(); i++) {
        int entry_inode_id = _dentries[i]->inode()->id();

        std::memcpy(current_entry, &entry_inode_id, sizeof(int));
        std::memcpy(current_entry + 4, _dentries[i]->name(), 16);

        current_entry += DIRECTORY_ENTRY_SIZE;
    }

    return c;
}



//Directory::Directory(char* content)
//{
//    int size, idid;
//    char* name = new char[16];
//    int entry_size = 4 + 16;
//
//    std::memcpy(&_parent_id, content,     4);   
//    std::memcpy(&_inode_id,  content + 4, 4);
//    std::memcpy(&size,       content + 8, 4);
//
//    _dentries = std::vector<DEntry*>();
//
//    char* entry = content + 12;
//    for (int i = 0; i < size; i++) {
//        std::memcpy(&idid, entry,     4);
//        std::memcpy(name,  entry + 4, 16);
//
//        _dentries.push_back(new DEntry(idid, name));
//        entry += entry_size;
//    }
//}