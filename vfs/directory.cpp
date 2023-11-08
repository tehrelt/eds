#define _CRT_SECURE_NO_WARNINGS
#include "directory.h"
#include "tools.h"
#include "dentry_factory.h"
#include "file_system.h"

void Directory::save()
{
    Storage::STORAGE()->writeBytes(_inode, 0, this->to_char(), DIRECTORY_RECORD_CHAR_SIZE);
}
Directory* Directory::parse(INode* inode, char* content, Directory* parent)
{
    int size, entry_inode_id;

    char* name = new char[12];

    std::memcpy(&size, content + 12, 4);


    Directory* dir = nullptr;
    if (parent == nullptr) {
        dir = new Directory(inode);
    }
    else {
        char* dir_name = new char[12];
        std::memcpy(dir_name, content, 12);
        dir = (Directory*)DEntryFactory::CREATE(DIRECTORY, inode, parent, dir_name);
    }

    char* entry = content + 16;
    for (int i = 0; i < size; i++) {

        std::memcpy(&entry_inode_id, entry, 4);
        std::memcpy(name, entry + 4, 12);

        INode* entry_inode = Storage::STORAGE()->getINode(entry_inode_id);

        dir->add(DEntryFactory::CREATE(entry_inode->IsDirectoryFlag() ? DIRECTORY : FILE, entry_inode, dir, name));

        entry += DIRECTORY_ENTRY_SIZE;
    }

    return dir;
}
void Directory::remove(DEntry* dentry)
{
    auto it = std::find(_dentries.begin(), _dentries.end(), dentry);
    _dentries.erase(it);
}
DEntry* Directory::findByName(const std::string& name)
{
    for (const auto& dentry : _dentries) {
        if (dentry->name() == name) {
            return dentry;
        }
    }
    return nullptr;
}

Directory::Directory(INode* inode)
    : DEntry(inode, nullptr, "")
{
    _dentries = std::vector<DEntry*>();
    _dentries.push_back(new DEntry(inode, this, "."));
}
Directory::Directory(INode* inode, DEntry* parent, const std::string& name)
    : DEntry(inode, parent, name)
{
    if (!inode->IsDirectoryFlag()) {
        throw std::exception();
    }

    _dentries = std::vector<DEntry*>();
}

void Directory::init()
{
    _dentries.push_back(new Directory(_inode, this, "."));
    _dentries.push_back(new Directory(_parent->inode(), this, ".."));
}

void Directory::add(DEntry* dentry)
{
    dentry->inode()->set_modify_date(getCurrentDate());
    Storage::STORAGE()->saveINode(dentry->inode());

    _dentries.push_back(dentry);
    save();
}

bool Directory::exists(const std::string& name)
{
    for (auto dentry : _dentries) {
        if (name == dentry->name()) {
            return true;
        }
    }
    return false;
}

File* Directory::createFile(std::string name, int uid)
{
    INode* inode = Storage::STORAGE()->allocateINode();

    inode->set_mode(0b110100);
    inode->set_uid(uid);
    Storage::STORAGE()->saveINode(inode);

    File* file = (File*)DEntryFactory::CREATE(FILE, inode, this, name);
    _dentries.push_back(file);

    this->save();

    return file;
}
File* Directory::getFile(std::string name)
{
    DEntry* dentry = findByName(name);

    if (dentry == nullptr) {
        throw no_such_file_exception(name, _name);
    }

    if (dentry->inode()->IsDirectoryFlag()) {
        throw std::exception("is not a file");
    }

    return (File*)dentry;
}
void Directory::removeFile(std::string name)
{
    DEntry* dentry = findByName(name);

    if (dentry == nullptr) {
        throw std::exception("no such file in directory");
    }

    if (dentry->getType() != FILE) {

    }

    this->remove(dentry);

    INode* inode = dentry->inode();
    Storage::STORAGE()->freeINode(inode);

    this->save();
}

void Directory::moveTo(DEntry* dentry, Directory* to)
{
    this->remove(dentry);
    to->add(dentry);

    this->save();
    to->save();
}

Directory* Directory::createDirectory(std::string name, int uid)
{
    INode* inode = Storage::STORAGE()->allocateINode();
    inode->SetDirectoryFlag();
    inode->set_uid(uid);
    Storage::STORAGE()->saveINode(inode);

    Directory* dir = (Directory*)DEntryFactory::CREATE(DIRECTORY, inode, this, name);
    _dentries.push_back(dir);

    this->save();
    dir->save();

    return dir;
}
Directory* Directory::getDirectory(std::string name)
{
    DEntry* dentry = nullptr;

    if (name == "..") {
        dentry = _parent;
    }
    else {
        dentry = findByName(name);
    }

    if (!exists(name)) {
        throw std::exception("no such directory");
    }

    if (dentry->inode()->IsDirectoryFlag() == false) {
        throw std::exception("is not a directory");
    }

    return Directory::READ(dentry);
}



char* Directory::to_char()
{
    char* c = new char[DIRECTORY_RECORD_CHAR_SIZE];

    int inode_id = _inode->id();

    std::memcpy(c, _name, 12);

    int offset = _parent == nullptr ? 1 : 2;

    int size = _dentries.size() - offset;
    std::memcpy(c + 12, &size, sizeof(int));

    char* current_entry = c + 16;

    for (int i = offset; i < _dentries.size(); i++) {
        int entry_inode_id = _dentries[i]->inode()->id();

        std::memcpy(current_entry, &entry_inode_id, sizeof(int));
        std::memcpy(current_entry + 4, _dentries[i]->name(), 12);

        current_entry += DIRECTORY_ENTRY_SIZE;
    }

    return c;
}

Directory* Directory::CREATE_ROOT()
{
    INode* root_inode = Storage::STORAGE()->allocateINode();
    root_inode->SetDirectoryFlag();
    root_inode->SetSystemFlag();
    Storage::STORAGE()->saveINode(root_inode);

    Directory* root = new Directory(root_inode);
    root->save();

    return root;
}
Directory* Directory::READ_ROOT()
{
    INode* root_inode = Storage::STORAGE()->getINode(0);
    char* data        = Storage::STORAGE()->readBytes(root_inode);
    Directory* root   = Directory::parse(root_inode, data, nullptr);

    delete data;
    return root;
}
Directory* Directory::READ(DEntry* dentry)
{
    INode* inode = dentry->inode();

    char* data = Storage::STORAGE()->readBytes(inode);
    Directory* root = Directory::parse(inode, data, (Directory*)dentry->parent());
    delete data;

    return root;
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