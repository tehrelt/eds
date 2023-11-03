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
    if (_current_directory != _root) {
        delete _current_directory;
    }
    _current_directory = dir;
}
void FileSystem::ChangeToRootDirectory()
{
    if (_current_directory != _root) {
        delete _current_directory;
    }
    _current_directory = _root;
}

char* FileSystem::GetBlockContent(int inode_id)
{
    INode* inode = _services->inode_service()->Get(inode_id);
    return _services->block_service()->Get(inode->block_num())->data();
}
Directory* FileSystem::GetDirectory(int inode_id)
{
    return _services->directory_service()->Get(inode_id);
}
Directory* FileSystem::GetParentDirectory()
{
    return _services->directory_service()->Get(_current_directory->parent());
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

File* FileSystem::CreateFile(std::string name)
{
    File* file = _services->file_service()->Create(name);
    Directory* dir = _services->directory_service()->AddToDirectory(_current_directory, new DEntry(file->inode()->id(), name));
    return file;
}
Directory* FileSystem::CreateDirectory(std::string name)
{
    Directory* dir = _services->directory_service()->Create(name, _current_directory);

    _services->directory_service()->AddToDirectory(_current_directory, new DEntry(dir->inode_id(), name));

    return dir;
}

User* FileSystem::CreateUser(std::string name, std::string password)
{
    return _services->user_service()->Create(name, password);
}

void FileSystem::Write(int inode_id, std::string text)
{
    INode* inode = _services->inode_service()->Get(inode_id);
    _services->block_service()->Write(inode, text);
    delete inode;
}

char* FileSystem::ReadFile(int inode_id)
{
    return _services->file_service()->Read(inode_id);
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
        throw new std::exception();
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

    fs->_root = fs->_services->directory_service()->CreateRoot();
    fs->_current_directory = fs->_root;

    INode* users_inode = fs->services()->file_service()->Create("usr")->inode();
    users_inode->SetSystemFlag();
    fs->services()->inode_service()->Save(users_inode);
    fs->_services->directory_service()->AddToDirectory(fs->_root, new DEntry(users_inode->id(), "usr"));

    std::string username;
    std::string pass;
    std::cout << "CREATE A ROOT USER: " << std::endl;
    std::cout << "password: ";  std::cin >> pass;
    fs->_root_user = fs->CreateUser("root", pass);
    
    std::cout << "CREATE A REGUALAR USER: " << std::endl;
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
        throw new std::exception("Невозможно открыть файл");
        return nullptr;
    }

    stream.read((char*)&sb, sizeof(Superblock));

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
    Storage* storage = new Storage(name, &sb, &fat, &imap);
    Service* service = new Service(storage);
    FileSystem* fs = new FileSystem(service);

    fs->_root = fs->_services->directory_service()->ReadRoot();
    fs->_current_directory = fs->_root;

    return fs;
}