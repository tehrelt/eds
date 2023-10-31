#include "terminal.h"

Terminal::Terminal()
{
    _current_path = "/";
}

int Terminal::Listen()
{
    std::string cmd;
    while (true) {
        std::cout << "user@eds " << _current_path << ": ";
        std::cin >> cmd;

        if (cmd == "ls") {

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
