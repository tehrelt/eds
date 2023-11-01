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
        std::cout << "user@eds " << "path_placeholder" << ": ";
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

    return 0;
}

int Terminal::mkdir(std::string name)
{
    if (name.length() == 0) {
        return -1;
    }


    return 0;
}

int Terminal::ls()
{
    return 0;
}
