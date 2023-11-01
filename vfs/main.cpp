#include <iostream>
#include <fstream>
#include "file_system.h"
#include "terminal.h"
#include "service.h"

Service* create_file_system(uint_fast64_t size, std::string name);
Service* mount_file_system(std::string name);

int main()
{
    Service* services = nullptr;
    std::string name;

    std::cout << "\t fs: EDS" << std::endl;
    std::cout << "[1] - Create" << std::endl;
    std::cout << "[2] - Mount" << std::endl;

    int choice;
    std::cout << "Enter an option: ";
    std::cin >> choice;

    if (choice == 1) {
        uint_fast64_t size_in_kb;
        std::cout << "Creating fs!" << std::endl;

        std::cout << "Enter a file name: ";
        std::cin >> name;

        std::cout << "Enter a size in kb: ";
        std::cin >> size_in_kb;

        services = create_file_system(size_in_kb << 10, name + ".eds");
    }
    else if (choice == 2) {
        std::cout << "Mounting fs" << std::endl;
        std::cout << "Enter a file name of existing fs: ";
        std::cin >> name;
        services = mount_file_system(name + ".eds");
    }
    else {
        std::cout << "INVALID OPTION" << std::endl;
        system("pause");
        return -1;
    }

    if (services == nullptr) {
        std::cout << "File system not initialized" << std::endl;
        return -1;
    }

    

    Terminal terminal = Terminal(services);

    return terminal.Listen();
}

Service* create_file_system(uint_fast64_t size, std::string name)
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

    FileSystem* fs = new FileSystem(name, sb);

    std::ofstream stream;

    stream.open(name, std::ios::binary | std::ios::out);

    if (!stream.is_open()) {
        throw new std::exception();
    }

    int block_size = sb.block_size();
    int fs_size_in_blocks = sb.fs_size_in_blocks();
    for (int i = 0; i < fs_size_in_blocks; i++) {
        Block* b = new Block(i, block_size);
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

    Service* s = new Service(fs);

    s->set_directory(s->directory_service()->CreateRoot());

    return s;
}
Service* mount_file_system(std::string name)
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
        Inode* inode = new Inode(i);
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

    BlockRepository block_repo = BlockRepository(name, &sb, fat);
    INodeRepository inode_repo = INodeRepository(name, &sb, imap);
    Storage storage = Storage(name, block_repo, inode_repo);

    FileSystem* fs = new FileSystem(name, sb, storage);

    return new Service(fs);
}
