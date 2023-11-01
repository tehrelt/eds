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
        std::cout << "user@eds " << "(path)" << ": ";
        std::cin >> cmd;

        if (cmd == "ls") {
            ls();
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

    return 0;
}
int Terminal::mkdir(std::string name)
{
    if (name.length() == 0) {
        return -1;
    }

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
    return 0;
}
