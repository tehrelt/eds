#include <iostream>
#include <fstream>
#include <string>
#include "file_system.h"
#include "terminal.h"
#include "tools.h"

int main()
{
    bool debug = false;

    std::ifstream config_stream;

    config_stream.open(".cfg");

    if (config_stream.is_open()) {
        std::string key;
        while (config_stream >> key)
        {
            std::string value;
            config_stream >> value;

            if (key == "debug") {
                debug = value == "true";
            }
        }
    }
    Log::IS_DEBUG = debug;

    Log log = Log("main");
    log.info("Logger initialized");
    log.debug("DEBUG MODE");

    FileSystem* fs = nullptr;
    std::string name;

    std::cout << "\t fs: EDS" << std::endl;
    std::cout << "[1] - Create" << std::endl;
    std::cout << "[2] - Mount" << std::endl;

    int choice;
    std::cout << "Enter an option: ";
    std::cin >> choice;
    try
    {
        if (choice == 1) {
            uint_fast64_t size_in_kb;
            std::cout << "Creating fs!" << std::endl;

            std::cout << "Enter a file name: ";
            std::cin >> name;

            std::cout << "Enter a size in kb: ";
            std::cin >> size_in_kb;

            fs = FileSystem::Create(name + ".eds", size_in_kb << 10);
        }
        else if (choice == 2) {
            std::cout << "Mounting fs" << std::endl;
            std::cout << "Enter a file name of existing fs: ";
            std::cin >> name;
            fs = FileSystem::Mount(name + ".eds");
        }
        else {
            std::cout << "INVALID OPTION" << std::endl;
            system("pause");
            return -1;
        }

        if (fs == nullptr) {
            std::cout << "File system not initialized" << std::endl;
            return -1;
        }

        Terminal terminal = Terminal(fs);

        return terminal.Listen();
    }
    catch (const std::exception& e )
    {
        std::cout << "Error has occured: " << e.what() << std::endl;
    }
}