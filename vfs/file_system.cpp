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
#include "sha256.h"

void FileSystem::init()
{
    _root_directory = Directory::CREATE_ROOT();
    _current_directory = _root_directory;

    File* users_file = _root_directory->createFile("usr", 0);
    INode* users_inode = users_file->inode();
    users_inode->SetSystemFlag();
    users_inode->set_uid(0);
    storage->saveINode(users_inode);

    std::string username;
    std::string pass;
    std::cout << "CREATE A ROOT USER: " << std::endl;
    std::cout << "password: ";  std::cin >> pass;
    _root_user = createUser("root", pass);

    std::cout << "CREATE A REGULAR USER: " << std::endl;
    std::cout << "name: ";      std::cin >> username;
    std::cout << "password: ";  std::cin >> pass;
    _current_user = createUser(username, pass);
}

Directory* FileSystem::forwardTo(Directory* to)
{
    _current_directory = to;
    return _current_directory;
}

User* FileSystem::createUser(const std::string& username, const std::string& pass)
{
    int id = storage->getNextUID();

    std::string hash_password = sha256(pass);

    User* user = new User(id, username);

    char* user_record = new char[FULL_USER_RECORD_SIZE];

    std::memcpy(user_record, user->to_char(), USER_RECORD_SIZE);
    std::memcpy(user_record + USER_RECORD_SIZE, hash_password.c_str(), 64);

    File* usersFile = _root_directory->getFile("usr");
    usersFile->seek(id * FULL_USER_RECORD_SIZE, 0);
    usersFile->write(user_record, FULL_USER_RECORD_SIZE);

    storage->addUser();

    return user;
}
User* FileSystem::getUserById(int id)
{
    char* users = _root_directory->getFile("usr")->read();
    
    char* user_char = new char[USER_RECORD_SIZE];

    std::memcpy(user_char, users + id * (FULL_USER_RECORD_SIZE), USER_RECORD_SIZE);

    User* user = new User(user_char);

    delete[] users;
    delete[] user_char;

    return user;
}
bool FileSystem::userExists(const std::string& username)
{
    char* users = _root_directory->getFile("usr")->read();
    char* name = new char[16];

    User* user = nullptr;

    for (int i = 0; i < storage->getNextUID(); i++)
    {
        int offset = i * FULL_USER_RECORD_SIZE;
        std::memcpy(name, users + offset + 4, 16);

        if (name == username) {
            return true;
        }
    }

    return false;
}
User* FileSystem::findUserByName(const std::string& username)
{
    char* users = _root_directory->getFile("usr")->read();

    char* user_char = new char[USER_RECORD_SIZE];
    char* name = new char[16];

    User* user = nullptr;

    for (int i = 0; i < storage->getNextUID(); i++)
    {
        int offset = i * FULL_USER_RECORD_SIZE;
        std::memcpy(name, users + offset + 4, 16);

        if (name == username) {
            std::memcpy(user_char, users + offset, USER_RECORD_SIZE);
            user = new User(user_char);
        }
    }

    return user;
}
bool FileSystem::login(const std::string& username, const std::string& pass)
{
    char* users = _root_directory->getFile("usr")->read();

    char* hash_pass = new char[65];
    hash_pass[64] = '\0';

    User* user = findUserByName(username);

    std::memcpy(hash_pass, users + user->id() * FULL_USER_RECORD_SIZE + USER_RECORD_SIZE, 64);

    if (hash_pass != sha256(pass)) {
        return false;
    }

    _current_user = user;

    return true;
}

bool FileSystem::checkOwner(INode* inode)
{
    return inode->uid() == _current_user->id() || _current_user->id() == 0;
}

Superblock* FileSystem::sb()
{
    return storage->superblock();
}

FileSystem* FileSystem::Create(std::string name, uint_fast64_t size)
{
    std::ofstream stream; 

    auto log = Log("FileSystem::Create");

    stream.open(name, std::ios::binary | std::ios::out);
    

    if (!stream.is_open()) {
        throw std::exception("Cannot create a file");
    }
    log.debug("opened a stream at " + name);

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

    Storage::INIT_STORAGE(name, &sb, &fat, &imap);

    FileSystem* fs = new FileSystem();

    int block_size = sb.block_size();
    int fs_size_in_blocks = sb.fs_size_in_blocks();
    for (int i = 0; i < fs_size_in_blocks; i++) {
        char* block = new char[block_size];
        stream.write(block, block_size);
        delete[] block;
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

    fs->init();
    
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

    fs->_root_directory = Directory::READ_ROOT();
    fs->_current_directory = fs->_root_directory;

    File* users_file = fs->_root_directory->getFile("usr");

    std::cout << "Login procedure" << std::endl;
    std::string username;
    std::string pass;
    do
    {
        std::cout << "login: ";         
        std::cin >> username;

        if (fs->findUserByName(username) == nullptr) {
            std::cout << "user not found. Please try again!" << std::endl;
            continue;
        }

        std::cout << "password: ";      
        std::cin >> pass;

        fs->login(username, pass);
    } while (fs->_current_user == nullptr);

    return fs;
}
