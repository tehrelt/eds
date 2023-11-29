#define _CRT_SECURE_NO_WARNINGS
#include "directory.h"
#include "tools.h"
#include "dentry_factory.h"
#include "file_system.h"

void Directory::save()
{
    storage->writeBytes(_inode, 0, this->to_char(), DIRECTORY_RECORD_CHAR_SIZE);
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

        INode* entry_inode = storage->getINode(entry_inode_id);

        dir->add(DEntryFactory::CREATE(entry_inode->IsDirectoryFlag() ? DIRECTORY : FILE, entry_inode, dir, name));

        entry += DIRECTORY_ENTRY_SIZE;
    }

    return dir;
}

void Directory::erase(DEntry* dentry)
{
    auto it = std::find(_dentries.begin(), _dentries.end(), dentry);
    _dentries.erase(it);
}
void Directory::erase(const std::string& name)
{
    auto dentry = this->findByName(name);
    this->erase(dentry);
}
DEntry* Directory::findByName(const std::string& name)
{
    for (const auto& dentry : _dentries) {
        if (dentry->name() == name) {
            return dentry;
        }
    }
    throw no_such_file_exception(name, _path.ToString());
}

Directory::Directory(INode* inode)
    : DEntry(inode, nullptr, "")
{
    _dentries = std::vector<DEntry*>();
    _dentries.push_back(new Directory(inode, this, "."));
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
    storage->saveINode(dentry->inode());

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
    if (_inode->uid() != uid && !_inode->is_____w_() && uid != 0) {
        throw std::exception("you cannot create files in this directory");
    }

    INode* inode = storage->allocateINode();

    inode->set_uid(uid);
    storage->saveINode(inode);

    File* file = (File*)DEntryFactory::CREATE(FILE, inode, this, name);

    _dentries.push_back(file);

    save();

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
void Directory::removeFile(std::string name, int uid)
{
    DEntry* dentry = findByName(name);

    if (dentry == nullptr) {
        throw std::exception("no such file in directory");
    } else if (dentry->getType() != FILE) {
        throw std::exception("cannot remove a directory");
    } else if (dentry->inode()->IsSystemFlag()) {
        throw std::exception("cannot remove system component");
    } else if (dentry->inode()->uid() != uid && uid != 0) {
        throw std::exception("cannot remove a component because you're not a owner or root-user");
    }

    this->erase(dentry);

    INode* inode = dentry->inode();
    storage->freeINode(inode);

    this->save();
}

void Directory::moveTo(DEntry* dentry, Directory* to, int uid)
{
    Log log("Directory::moveTo");

    if (dentry->inode()->IsSystemFlag()) {
        throw std::exception("cannot copy system component");
    } 

    this->erase(dentry);
    to->add(dentry);

    this->save();
    to->save();

    log.info("moved '" + dentry->path()->ToString() + "' to '" + to->path()->ToString() + "'");
}

void Directory::copyTo(Directory* destination, int uid)
{
    Log log("Directory::copyTo");

    if (this->_inode->IsSystemFlag()) {
        throw std::exception("cannot copy system component");
    }

    int offset = _inode->id() == 0 ? 1 : 2;
    int i = 0;
    for (auto& dentry : _dentries)
    {
        if (i < offset) {
            i++;
            continue;
        }

        std::string name = dentry->name();

        if (dentry->getType() == DIRECTORY) {

            Directory* subdir = this->getDirectory(name);
            Directory* cp_subdir = destination->createDirectory(name, dentry->inode()->uid());
            log.info("copy '" + subdir->path()->ToString() + "' to '" + cp_subdir->path()->ToString() + "'");
            subdir->copyTo(cp_subdir, uid);

        }
        else {
            File* file = this->getFile(name);
            try
            {
                this->copyTo(file, destination, file->name(), uid);
            }
            catch (const std::exception&)
            {
                log.warn("cannot copy '" + name + "' to '" + destination->path()->ToString() + "'");
            }
        }
        i++;
    }
}
void Directory::copyTo(File* file, Directory* destination, const std::string& file_name, int uid)
{
    Log log("Directory::copyTo");

    if (file->inode()->uid() != uid) {
        if (!file->inode()->is____r__()) {
            throw std::exception("copy: permission denied");
        }
    }

    if (destination->exists(file_name)) {
        throw std::exception("copy: element already exists");
    }

    File* cp_file = destination->createFile(file_name, uid);
    cp_file->write(file->read(), file->length());
    log.info("copied '" + file->path()->ToString() + "' to '" + cp_file->path()->ToString() + "'");
}

Directory* Directory::createDirectory(std::string name, int uid)
{
    if (_inode->uid() != uid && !_inode->is_____w_() && uid != 0) {
        throw std::exception("you cannot create files in this directory");
    }

    INode* inode = storage->allocateINode();
    inode->SetDirectoryFlag();
    inode->set_uid(uid);
    storage->saveINode(inode);

    Directory* dir = (Directory*)DEntryFactory::CREATE(DIRECTORY, inode, this, name);
    _dentries.insert(_dentries.begin() + 2, dir);

    this->save();
    dir->save();

    return dir;
}
Directory* Directory::getDirectory(std::string name)
{
    DEntry* dentry = nullptr;

    if (name == ".") {
        dentry = this;
    }
    else if (name == "..") {
        dentry = _parent;
    }
    else {
        dentry = findByName(name);
    }

    if (dentry == nullptr) {
        throw std::exception("no such directory");
    }

    if (dentry->getType() != DIRECTORY) {
        throw std::exception("is not a directory");
    }

    return Directory::READ(dentry);
}
void Directory::removeDirectory(const std::string& name, int uid)
{
    Log log = Log("Directory::removeDirectory");
    Directory* dir = this->getDirectory(name);

    if (dir == nullptr) {
        throw std::exception("no such file in directory");
    }
    else if (dir->getType() != DIRECTORY) {
        throw std::exception("cannot remove a file with this command");
    }
    else if (dir->inode()->IsSystemFlag()) {
        throw std::exception("cannot remove system component");
    }
    else if (dir->inode()->uid() != uid && uid != 0) {
        throw std::exception("cannot remove a component because you're not a owner or root-user");
    }

    dir->remove(uid);

    if (dir->dentries().size() == 2) {
        log.info("removing '" + name + "'(" + std::to_string(dir->inode()->block_num()) + ") at " + _path.ToString());
        this->erase(name);
        storage->freeINode(dir->inode());
        delete dir;
    }
    else {
        throw std::exception("cannot remove dir isn't empty");
    }
    


    this->save();
}

void Directory::remove(int uid)
{
    Log log = Log("Directory::remove");

    int offset = _inode->id() == 0 ? 1 : 2;
    int i = _dentries.size() - 1;

    while (i > 1)
    {

        auto& dentry = _dentries[i];
        std::string name = dentry->name();        

        log.info("removing '" + name + "'(" + std::to_string(dentry->inode()->block_num()) + ") at " + _path.ToString());

        if (dentry->getType() == DIRECTORY) {
            /*Directory* subdir = this->getDirectory(name);
            
            subdir->remove(uid);*/
            try {
                removeDirectory(name, uid);
            }
            catch (const std::exception& e) {
                log.warn("cannot remove '" + name + "' at '" + _path.ToString() + "'");
            }

            /*if (subdir->_dentries.size() > 2) {
                this->erase(name);
                storage->freeINode(subdir->inode());
                this->save();
                delete subdir;
            }*/
        }
        else {
            try {
                this->removeFile(name, uid);
            }
            catch (const std::exception& e) {
                log.warn("cannot remove '" + name + "' at '" + _path.ToString() + "'");
            }
        }
        i--;
    }
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
    INode* root_inode = storage->allocateINode();
    root_inode->SetDirectoryFlag();
    root_inode->SetSystemFlag();
    storage->saveINode(root_inode);

    Directory* root = new Directory(root_inode);
    root->save();

    return root;
}
Directory* Directory::READ_ROOT()
{
    INode* root_inode = storage->getINode(0);
    char* data        = storage->readBytes(root_inode);
    Directory* root   = Directory::parse(root_inode, data, nullptr);

    delete data;
    return root;
}
Directory* Directory::READ(DEntry* dentry)
{
    INode* inode = dentry->inode();

    char* data = storage->readBytes(inode);
    Directory* root = Directory::parse(inode, data, (Directory*)dentry->parent());
    delete data;

    return root;
}

std::ostream& operator<<(std::ostream& os, Directory* d)
{
    os << d->_path.ToString();
    return os;
}
