#include <iostream>
#include <fstream>
#include <sstream>
#include "file_system.h"
#include "block.h"
#include "tools.h"

void FileSystem::write(char* source, int offset, int size)
{
    std::fstream stream;

    stream.open(_file_name, std::ios::binary | std::ios::out | std::ios::in);

    if (!stream.is_open())
        throw new std::exception();


    stream.seekp(offset, std::ios::beg);
    stream.write(source, size);

    stream.close();
}

void FileSystem::read(char* destination, int offset, int size)
{
    std::ifstream stream;

    stream.open(_file_name, std::ios::binary | std::ios::in);

    if (!stream.is_open())
        throw new std::exception();

    stream.seekg(offset, std::ios::beg);
    stream.read(destination, size);

    stream.close();
}

void FileSystem::saveBlock(Block block, int index)
{
    int offset = (_superblock.num_of_first_data_block() + index) * _superblock.block_size();
    write(block.data(), offset, _superblock.block_size());
}

void FileSystem::saveInode(int index)
{
    Inode inode = _imap[index];
    write((char*)&inode, (_superblock.num_of_first_imap_block() * _superblock.block_size()) + (index * sizeof(Inode)), sizeof(Inode));
    
    uint_fast32_t part_idx = _imap.get_part_index(index);
    uint_fast64_t part = _imap.part(index);
    write((char*)&part, (_superblock.num_of_first_part_block() * _superblock.block_size()) + part_idx * sizeof(uint_fast64_t), sizeof(uint_fast64_t));
}

void FileSystem::saveFAT(int index)
{
    int_fast32_t record = _fat[index];

    int offset = (_superblock.num_of_first_fat_block() * _superblock.block_size()) + index * sizeof(int_fast32_t);

    write((char*)&record, offset, sizeof(int_fast32_t));
}

Inode* FileSystem::allocateInode(int index)
{
    Inode* inode = &_imap[index];

    _imap.Lock(inode->id());
    inode->set_create_date(getCurrentDate());

    return inode;
}

void FileSystem::allocateFile()
{
    int block_idx = findFreeBlockNum();
    int inode_idx = findFreeInodeNum();

    Inode* inode = allocateInode(inode_idx);

    inode->set_block_num(block_idx);
    _fat[block_idx] = -2;

    saveInode(inode_idx);
    saveFAT(block_idx);
}

int FileSystem::findFreeBlockNum()
{
    for (int i = 0; i < _fat.capacity(); i++) {
        if (_fat[i] == -1) {
            return i;
        }
    }

    return -1;
}
int FileSystem::findFreeInodeNum()
{
    for (int i = 0; i < _imap.capacity(); i++) {
        if (_imap.IsLocked(i) == false) {
            return i;
        }
    }

    return -1;
}

FileSystem::FileSystem(std::string name, Superblock* superblock, FAT* fat, IMap* ilist)
{
    _file_name = name;
    _superblock = *superblock;
    _fat = *fat;
    _imap = *ilist;
}

FileSystem* FileSystem::Create(int size, std::string name)
{
    Superblock sb = Superblock(size);
    FAT fat = FAT(sb.fat_capacity());
    IMap imap = IMap(sb.imap_capacity());

    FileSystem* fs = new FileSystem(name, &sb, &fat, &imap);

    std::ofstream stream;

    stream.open(name, std::ios::binary | std::ios::out);

    if (!stream.is_open()) {
        throw new std::exception();
    }

    int block_size = sb.block_size();
    int fs_size_in_blocks = sb.fs_size_in_blocks();
    for (int i = 0; i < fs_size_in_blocks; i++) {
        Block* b = new Block(block_size);
        stream.write((char*)b, block_size);
        delete b;
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
        Inode* inode = new Inode(i);
        stream.write((char*)inode, sizeof(Inode));
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


    fs->allocateFile();

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
        Inode *inode = new Inode(i);
        stream.read((char*)inode, sizeof(Inode));
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

    FileSystem* fs = new FileSystem(name, &sb, &fat, &imap);

    return fs;
}

std::string FileSystem::ToString()
{ 
    std::stringstream stream;

    int inodes_count = _imap.capacity();
    int blocks_count = _fat.capacity();

    stream << "######## SUPERBLOCK ########" << std::endl;
    stream << "\tfile system name: " << _superblock.fs_name() << std::endl; 
    stream << "\tblock size: " << _superblock.block_size() << std::endl;
    stream << "\tFAT\tcapacity: " << _superblock.fat_capacity() << "\tsize: " << _superblock.fat_size() << " bytes" << "\tfirst block: " << _superblock.num_of_first_fat_block() << std::endl;
    stream << "\tIMap\tcapacity: " << _superblock.imap_capacity() << "\tsize: " << _superblock.imap_size() << " bytes" << "\tfirst block: " << _superblock.num_of_first_imap_block() << std::endl;
    stream << "\tIBitmap\tparts: " << _superblock.imap_parts_count() << "\tsize: " << _superblock.imap_parts_size() << " bytes" << std::endl;
    stream << "\tBlocks\tcapacity: " << _superblock.data_blocks_count() << std::endl;
    stream << "\tSpace\tfree space: " << _superblock.free_space_in_bytes() << " bytes" << std::endl;
    stream << "\t\ttotal space: " << _superblock.total_space_in_bytes() << " bytes" << std::endl;

    stream << "######## INODES ########" << std::endl;
    stream << "id\tlock\tcreation date\tfirst block" << std::endl;
    for (int i = 0; i < inodes_count; i++) {
        int id = _imap[i].id();
        stream << id << " \t " << std::to_string(_imap.IsLocked(id)) << "\t" << timeToString(_imap[i].create_date()) << "\t" << _imap[i].block_num() << std::endl;
    }

    stream << "######## BLOCKS ########" << std::endl;
    stream << "id\tnext" << std::endl;
    for (int i = 0; i < blocks_count; i++) {
        stream << i << "\t" << _fat[i] << std::endl;
    }

    return stream.str();
}
