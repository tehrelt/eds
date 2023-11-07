#include <iostream>
#include <fstream>
#include <sstream>
#include "file_system.h"
#include "block.h"
#include "tools.h"
#include "inode.h"
#include "fat.h"
#include "imap.h"
#include "superblock.h"
#include "storage.h"

FileSystem::FileSystem()
{
    _services = nullptr;
}
FileSystem::FileSystem(Service* services)
{
    _services = services;
}

void FileSystem::ChangeDirectory(Directory* dir)
{    
    _current_directory = _services->directory_service()->Get(dir->inode(), (Directory*)dir->parent());
}

void FileSystem::ChangeToRootDirectory()
{
    if (_current_directory != _root_directory) {
        delete _current_directory;
    }
    _current_directory = _root_directory;
}

char* FileSystem::GetBlockContent(int inode_id)
{
    INode* inode = _services->inode_service()->Get(inode_id);
    return _services->block_service()->Get(inode->block_num())->data();
}

Directory* FileSystem::GetDirectory(INode* inode, Directory* parent)
{
    return _services->directory_service()->Get(inode, parent);
}

Directory* FileSystem::GetParentDirectory()
{
    if (_current_directory->parent() == nullptr) {
        throw std::exception("YOU ARE ALREADY IN ROOT DIRECTORY!");
    }
    
    DEntry* p = _current_directory->parent();
    Directory* parent = _services->directory_service()->Get(p->inode(), (Directory*)p->parent());

    return parent;
}
Directory* FileSystem::GetParentDirectory(Directory* at)
{
    if (at->parent() == nullptr) {
        throw std::exception("YOU ARE ALREADY IN ROOT DIRECTORY!");
    }

    DEntry* p = at->parent();
    Directory* parent = _services->directory_service()->Get(p->inode(), (Directory*)p->parent());

    return parent;
}

void FileSystem::GoBack()
{

}

Block* FileSystem::GetBlock(int id)
{
    return _services->block_service()->Get(id);
}
INode* FileSystem::GetInode(int id)
{
    try
    {
        return _services->inode_service()->Get(id);
    }
    catch (const std::exception& e)
    {
        throw e;
    }
}

File* FileSystem::CreateFile(std::string name, Directory* at)
{
    File* file = _services->file_service()->Create(name, at);
    
    INode* inode = file->inode();
    _services->inode_service()->SetOwner(inode, _current_user->id());
    _services->inode_service()->SetMode(inode, 0b110100);               // rw-r--
    _services->inode_service()->Save(inode);

    _services->directory_service()->AddToDirectory(at, file);
    return file;
}
void FileSystem::RemoveFile(DEntry* dentry)
{
    _services->file_service()->Remove(dentry);
    _services->directory_service()->RemoveFromDirectory(_current_directory, dentry);
}
void FileSystem::RemoveFile(DEntry* dentry, Directory* at)
{
    _services->file_service()->Remove(dentry);
    _services->directory_service()->RemoveFromDirectory(at, dentry);
}

Directory* FileSystem::CreateDirectory(std::string name, Directory* at)
{
    Directory* dir = _services->directory_service()->Create(name, at);
    INode* inode = dir->inode();

    _services->inode_service()->SetOwner(inode, _current_user->id());
    _services->inode_service()->SetMode(inode, 0b110100);               // rw-r--
    _services->inode_service()->Save(inode);

    _services->directory_service()->AddToDirectory(at, dir);

    return dir;
}

User* FileSystem::CreateUser(std::string name, std::string password)
{
    return _services->user_service()->Create(name, password);
}
User* FileSystem::GetUser(int id)
{
    char* users = _services->file_service()->Read((File*)_root_directory->dentries()[USERS_INODE]);
    char* userc = new char[USER_RECORD_SIZE];
    std::memcpy(userc, users + id*(USER_RECORD_SIZE), USER_RECORD_SIZE);

    User* user = new User(userc);

    delete users;
    delete userc;

    return user;
}

void FileSystem::AppendFile(File* file, std::string text)
{
    _services->file_service()->Append(file, text);
}
void FileSystem::WriteFile(File* file, std::string text)
{
    _services->file_service()->Write(file, text);
}

char* FileSystem::ReadFile(File* file)
{
    return _services->file_service()->Read(file);
}
std::vector<DEntry*> FileSystem::ls()
{
    return _services->directory_service()->GetInfo(_current_directory);
}
Superblock* FileSystem::sb()
{
    return _services->block_service()->GetSB();
}

FileSystem* FileSystem::Create(std::string name, uint_fast64_t size)
{
    Superblock sb = Superblock(size);

    FAT fat = FAT(sb.fat_capacity());
    IMap imap = IMap(sb.imap_capacity());


    for (int i = 0; i < sb.num_of_first_data_block(); i++) {
        if (i >= sb.num_of_first_fat_block() && i < sb.num_of_first_imap_block() - 1) {
            fat[i] = i + 1;
        }
        else if (i >= sb.num_of_first_imap_block() && i < sb.num_of_first_part_block() - 1) {
            fat[i] = i + 1;
        }
        else if (i >= sb.num_of_first_part_block() && i < sb.num_of_first_data_block() - 1) {
            fat[i] = i + 1;
        }
        else {
            fat[i] = -2;
        }
    }

    Storage* storage = new Storage(name, &sb, &fat, &imap);
    Service* service = new Service(storage);
    FileSystem* fs = new FileSystem(service);

    std::ofstream stream;

    stream.open(name, std::ios::binary | std::ios::out);

    if (!stream.is_open()) {
        throw std::exception("Cannot create a file");
    }

    int block_size = sb.block_size();
    int fs_size_in_blocks = sb.fs_size_in_blocks();
    for (int i = 0; i < fs_size_in_blocks; i++) {
        char* block = new char[block_size];
        stream.write(block, block_size);
        delete block;
    }

    stream.seekp(0);
    stream.write((char*)&sb, sizeof(Superblock));

    stream.seekp(sb.num_of_first_fat_block() * block_size);
    int fat_capacity = sb.fat_capacity();
    for (int i = 0; i < fat_capacity; i++) {
        stream.write((char*)&fat[i], sizeof(uint32_t));
    }

    stream.seekp(sb.num_of_first_imap_block() * block_size);
    int imap_capacity = sb.imap_capacity();
    for (int i = 0; i < imap_capacity; i++) {
        INode* inode = new INode(i);
        stream.write((char*)inode, sizeof(INode));
        imap[i] = *inode;
        
        delete inode;
    }

    stream.seekp(sb.num_of_first_part_block() * block_size);
    int parts_count = sb.imap_parts_count();
    for (int i = 0; i < parts_count; i++) {
        uint_fast64_t t = 0;
        stream.write((char*)&t, sizeof(uint_fast64_t));
    }

    stream.close();

    fs->_root_directory = fs->_services->directory_service()->CreateRoot();
    fs->_current_directory = fs->_root_directory;

    File* users_file = fs->services()->file_service()->Create("usr", fs->_root_directory);

    INode* users_inode = users_file->inode();
    users_inode->SetSystemFlag();
    users_inode->set_uid(0);
    fs->services()->inode_service()->Save(users_inode);

    fs->_services->directory_service()->AddToDirectory(fs->_root_directory, users_file);

    std::string username;
    std::string pass;
    std::cout << "CREATE A ROOT USER: " << std::endl;
    std::cout << "password: ";  std::cin >> pass;
    fs->_root_user = fs->CreateUser("root", pass);
    
    std::cout << "CREATE A REGULAR USER: " << std::endl;
    std::cout << "name: ";      std::cin >> username;
    std::cout << "password: ";  std::cin >> pass;
    fs->_current_user = fs->CreateUser(username, pass);
    
    return fs;
}
FileSystem* FileSystem::Mount(std::string name)
{
    Superblock sb = Superblock();

    std::ifstream stream;

    stream.open(name, std::ios::binary | std::ios::in);

    if (!stream.is_open()) {
        throw std::exception("Cannot open a file");
    }

    stream.read((char*)&sb, sizeof(Superblock));

    if (strcmp(sb.fs_name(), "EDS") != 0) {
        throw std::exception("Invalid superblock signature");
    }

    int block_size = sb.block_size();

    FAT fat = FAT(sb.fat_capacity());


    IMap imap = IMap(sb.imap_capacity());

    stream.seekg(sb.num_of_first_fat_block() * block_size);
    int fat_capacity = sb.fat_capacity();
    for (int i = 0; i < fat_capacity; i++) {
        stream.read((char*)&fat[i], sizeof(uint32_t));
    }

    stream.seekg(sb.num_of_first_imap_block() * block_size);
    int imap_capacity = sb.imap_capacity();
    for (int i = 0; i < imap_capacity; i++) {
        INode *inode = new INode(i);
        stream.read((char*)inode, sizeof(INode));
        imap.set_inode(i, inode);
        delete inode;
    }

    stream.seekg(sb.num_of_first_part_block() * block_size);
    int parts_count = sb.imap_parts_count();
    for (int i = 0; i < parts_count; i++) {
        uint_fast64_t part = 0;
        stream.read((char*)&part, sizeof(uint_fast64_t));
        imap.set_part(i, part);
    }

    stream.close();

    Storage::INIT_STORAGE(name, &sb, &fat, &imap);

    FileSystem* fs = new FileSystem();

    fs->_root_directory = fs->_services->directory_service()->ReadRoot();
    fs->_current_directory = fs->_root_directory;

    fs->_root_user = fs->_services->user_service()->Read(0);
    for (int i = 1; i < sb.users_count(); i++) {
        std::cout << i << ". " << fs->GetUser(i)->name() << std::endl;
    }

    int pick;

    do
    {
        std::cout << "Choose an user: ";
        std::cin >> pick;

        if (pick < 0 || pick > sb.users_count()) {
            std::cout << "Invalid user. Try again!" << std::endl;
        }
        else {
            fs->_current_user = fs->GetUser(pick);
        }
    } while (fs->current_user() == nullptr);

    return fs;
}