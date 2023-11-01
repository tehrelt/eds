#include "terminal.h"


Terminal::Terminal()
{
    _services = nullptr;
}

Terminal::Terminal(Service* services)
{
    _services = services;
}

int Terminal::Listen()
{
    std::string cmd;
    while (true) {
        std::cout << "user@eds " << _services->current_directory() << ": ";
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

    _services->file_service()->Create(name);

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

    return 0;
}
int Terminal::get_inode(int id)
{
    return 0;
}
int Terminal::ls()
{
    std::cout << "id\tflags\tmode\creation date\t" << std::endl;
    _services->directory_service()->Get(_services->current_directory());
    return 0;
}
