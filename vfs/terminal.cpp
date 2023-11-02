#include "terminal.h"


Terminal::Terminal()
{
    _file_system = nullptr;
}

Terminal::Terminal(FileSystem* file_system)
{
    _file_system = file_system;

    path = Path();
    path.add("");

    _commands = std::map<std::string, std::function<void()>>();

    _commands["ls"]     = std::bind(&Terminal::ls, this);
    _commands["mkdir"]  = std::bind(&Terminal::mkdir, this);
    _commands["mkfile"] = std::bind(&Terminal::mkfile, this);
    _commands["sb"]     = std::bind(&Terminal::sb, this);
    _commands["gi"]     = std::bind(&Terminal::get_inode, this);
    _commands["gb"]     = std::bind(&Terminal::get_block, this);
    _commands["cd"]     = std::bind(&Terminal::change_directory, this);

    _commands.emplace("cls",        []() { system("cls"); });
    _commands.emplace("shutdown",   []() { std::cout << "Shutdowning..."; });
}

int Terminal::Listen()
{
    std::string cmd;
    while (true) {
        std::cout << "user@eds " << path.ToString() << ": ";
        std::cin >> cmd;

        this->execute_command(cmd);

        if (cmd == "shutdown") {
            return 0;
        }
    }

    return 0;
}

void Terminal::execute_command(const std::string& cmd) {
    if (_commands.find(cmd) != _commands.end()) {
        _commands[cmd]();
    }
    else {
        std::cout << "Unknown command" << std::endl;
    }
}

void Terminal::mkfile()
{
    std::string name;
    std::cout << "file name: ";
    std::cin >> name;

    if (name.length() == 0 && name.length() > 16) {
        return;
    }

    _file_system->CreateFile(name);
}
void Terminal::mkdir()
{
    std::string name;
    std::cout << "dir name: ";
    std::cin >> name;

    if (name.length() == 0) {
    }

    _file_system->CreateDirectory(name);
}
void Terminal::sb()
{
    Superblock* sb = _file_system->sb();

    std::cout << "######## SUPERBLOCK ########" << std::endl;
    std::cout << "\tfile system name: " << sb->fs_name() << std::endl;
    std::cout << "\tblock size: " << sb->block_size() << std::endl;
    std::cout << "\tFAT\tcapacity: " << sb->fat_capacity() << "\tsize: " << sb->fat_size() << " bytes" << "\tfirst block: " << sb->num_of_first_fat_block() << std::endl;
    std::cout << "\tIMap\tcapacity: " << sb->imap_capacity() << "\tsize: " << sb->imap_size() << " bytes" << "\tfirst block: " << sb->num_of_first_imap_block() << std::endl;
    std::cout << "\tIBitmap\tparts: " << sb->imap_parts_count() << "\tsize: " << sb->imap_parts_size() << " bytes" << "\tfirst block " << sb->num_of_first_part_block() << std::endl;
    std::cout << "\tBlocks\tcapacity: " << sb->data_blocks_count() << "\tsize: " << sb->block_size() << " bytes" << "\tfirst block " << sb->num_of_first_data_block() << std::endl;
    std::cout << "\tSpace\tfree space: " << sb->free_space_in_bytes() << " bytes" << std::endl;
    std::cout << "\t\ttotal space: " << sb->total_space_in_bytes() << " bytes" << std::endl;
}
void Terminal::get_block()
{
    int id;
    std::cout << "id: ";
    std::cin >> id;
    try
    {
        Block* block = _file_system->GetBlock(id);
        std::cout << "Block info: " << std::endl;
        std::cout << *block;
    }
    catch (const std::exception& e)
    {
        std::cout << "При выполнении произошла ошибка: " << e.what() << std::endl;
    }
}
void Terminal::get_inode()
{
    int id;
    std::cout << "id: ";
    std::cin >> id;

    try
    {
        INode* inode = _file_system->GetInode(id);
        std::cout << "INODE INFO: " << std::endl;
        std::cout << *inode << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "При выполнении произошла ошибка: " << e.what() << std::endl;
    }
}
void Terminal::ls()
{
    auto vector = _file_system->ls();
    std::cout << "id\tflags\tmode\tcreation date\t\tname" << std::endl;

    for (int i = 0; i < vector.size(); i++) {
        INode* inode = _file_system->GetInode(vector[i]->inode_id());
        std::cout << *inode << "\t" << vector[i]->name() << std::endl;
    }
}
void Terminal::change_directory()
{
    std::string name;
    std::cout << "directory name to change: ";
    std::cin >> name;

    if (name == "..") {
        Directory* dir = _file_system->GetParentDirectory();
        _file_system->ChangeDirectory(dir);

        path.remove();
        
        return;
    }
    else if (name == "/") {
        _file_system->ChangeToRootDirectory();
        path = Path();
        path.add("");
        return;
    }

    for (auto dentry : _file_system->current_directory()->dentry()) {
        if (name == dentry->name()) {
            INode* inode = _file_system->GetInode(dentry->inode_id());

            if (inode->IsDirectoryFlag() == false) {
                std::cout << "ERROR! not a directory" << std::endl;
                return;
            }

            Directory* dir = _file_system->GetDirectory(inode->id());
            _file_system->ChangeDirectory(dir);
            path.add(dentry->name());
            return;
        }
    }
}
