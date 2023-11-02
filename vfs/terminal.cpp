#include "terminal.h"


Terminal::Terminal()
{
    _file_system = nullptr;
}

Terminal::Terminal(FileSystem* file_system)
{
    _file_system = file_system;
}

int Terminal::Listen()
{
    std::string cmd;
    while (true) {
        std::cout << "user@eds " << _file_system->current_directory()->path() << "/" << ": ";
        std::cin >> cmd;

        if (cmd == "ls") {
            ls();
        }
        else if (cmd == "sb") {
            sb();
        }
        else if (cmd == "mkf") {
            std::string name;
            std::cout << "file name: ";
            std::cin >> name;
            mkfile(name);
        }
        else if (cmd == "mkd") {
            std::string name;
            std::cout << "dir name: ";
            std::cin >> name;
            mkdir(name);
        }
        else if (cmd == "gi") {
            int id;
            std::cout << "id: ";
            std::cin >> id;
            get_inode(id);
        }
        else if (cmd == "gb") {
            int id;
            std::cout << "id: ";
            std::cin >> id;
            get_block(id);
        }
        else if (cmd == "shutdown") {
            std::cout << "Shutdowning...";
            return 0;
        }
        else if (cmd == "cls") {
            system("cls");
        }
        else {
            std::cout << "Invalid command" << std::endl;
        }
    }

    return 0;
}

int Terminal::mkfile(std::string name)
{
    if (name.length() == 0) {
        return -1;
    }

    _file_system->CreateFile(name);

    return 0;
}
int Terminal::mkdir(std::string name)
{
    if (name.length() == 0) {
        return -1;
    }

    _file_system->CreateDirectory(name);

    return 0;
}
int Terminal::sb()
{
    Superblock* sb = _file_system->sb();

    std::cout << "######## SUPERBLOCK ########" << std::endl;
    std::cout << "\tfile system name: " << sb->fs_name() << std::endl;
    std::cout << "\tblock size: " << sb->block_size() << std::endl;
    std::cout << "\tFAT\tcapacity: " << sb->fat_capacity() << "\tsize: " << sb->fat_size() << " bytes" << "\tfirst block: " << sb->num_of_first_fat_block() << std::endl;
    std::cout << "\tIMap\tcapacity: " << sb->imap_capacity() << "\tsize: " << sb->imap_size() << " bytes" << "\tfirst block: " << sb->num_of_first_imap_block() << std::endl;
    std::cout << "\tIBitmap\tparts: " << sb->imap_parts_count() << "\tsize: " << sb->imap_parts_size() << " bytes" << std::endl;
    std::cout << "\tBlocks\tcapacity: " << sb->data_blocks_count() << std::endl;
    std::cout << "\tSpace\tfree space: " << sb->free_space_in_bytes() << " bytes" << std::endl;
    std::cout << "\t\ttotal space: " << sb->total_space_in_bytes() << " bytes" << std::endl;

    return 0;
}
int Terminal::get_block(int id)
{
    try
    {
        Block* block = _file_system->GetBlock(id);
        std::cout << "Block info: " << std::endl;
        std::cout << *block;

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cout << "При выполнении произошла ошибка: " << e.what() << std::endl;
        return -1;
    }
}
int Terminal::get_inode(int id)
{
    try
    {
        INode* inode = _file_system->GetInode(id);
        std::cout << "INODE INFO: " << std::endl;
        std::cout << *inode << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "При выполнении произошла ошибка: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
int Terminal::ls()
{
    auto vector = _file_system->ls();

    for (int i = 0; i < vector.size(); i++) {
        INode* inode = _file_system->GetInode(vector[i]->inode_id());
        std::cout << *inode << "\t" << vector[i]->name() << std::endl;
    }

    return 0;
}
