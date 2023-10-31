#include <iostream>
#include <fstream>
#include "file_system.h"

int main()
{
    FileSystem* fs = nullptr;
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

        fs = FileSystem::Create(size_in_kb << 10, name + ".eds");
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

    std::cout << fs->ToString();

    return fs->Run();
}