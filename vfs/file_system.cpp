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

//void FileSystem::saveInode(int index)
//{
//    Inode inode = _imap[index];
//    write((char*)&inode, (_superblock.num_of_first_imap_block() * _superblock.block_size()) + (index * sizeof(Inode)), sizeof(Inode));
//    
//    uint_fast32_t part_idx = _imap.get_part_index(index);
//    uint_fast64_t part = _imap.part(index);
//    write((char*)&part, (_superblock.num_of_first_part_block() * _superblock.block_size()) + part_idx * sizeof(uint_fast64_t), sizeof(uint_fast64_t));
//}
//void FileSystem::saveInode(Inode* inode)
//{
//    write((char*)inode, (_superblock.num_of_first_imap_block() * _superblock.block_size()) + (inode->id() * sizeof(Inode)), sizeof(Inode));
//
//    uint_fast32_t part_idx = _imap.get_part_index(inode->id());
//    uint_fast64_t part = _imap.part(inode->id());
//    write((char*)&part, (_superblock.num_of_first_part_block() * _superblock.block_size()) + part_idx * sizeof(uint_fast64_t), sizeof(uint_fast64_t));
//}
//
//Inode* FileSystem::AllocateInode()
//{
//    int block_idx = findFreeBlockNum();
//    int inode_idx = findFreeInodeNum();
//
//    Inode* inode = &_imap[inode_idx];
//
//    _imap.Lock(inode->id());
//    inode->set_create_date(getCurrentDate());
//
//    inode->set_block_num(block_idx);
//    _fat[block_idx] = -2;
//
//    saveInode(inode_idx);
//    saveFAT(block_idx);
//    
//    return inode;
//}
//Inode* FileSystem::AllocateDir()
//{
//    Inode* inode = AllocateInode();
//    inode->SetDirectoryFlag();
//
//    saveInode(inode);
//
//    return inode;
//}
//
//void FileSystem::WriteIntoBlock(char* content, int block_idx)
//{
//    int offset = (_superblock.num_of_first_data_block() + block_idx) * _superblock.block_size();
//    write(content, offset, strlen(content));
//}



FileSystem::FileSystem()
{
    _services = nullptr;
}
FileSystem::FileSystem(Service* services)
{
    _services = services;
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

    return fs;
}
FileSystem* FileSystem::Mount(std::string name)
{
    Superblock sb = Superblock();

    std::ifstream stream;

    stream.open(name, std::ios::binary | std::ios::in);

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

    return fs;
}

// int FileSystem::Run() { return _terminal.Listen(); }