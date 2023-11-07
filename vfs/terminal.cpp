#include "terminal.h"
#include "tools.h"
#include <sstream>


Terminal::Terminal()
{
    _file_system = nullptr;
}

Terminal::Terminal(FileSystem* file_system)
{
    _file_system = file_system;

    path = Path();
    path.add("");

    _commands = std::map<std::string, std::function<void(std::vector<std::string>, Directory*)>>();

    _commands["ls"]     = std::bind(&Terminal::ls,               this, std::placeholders::_1, std::placeholders::_2);
    _commands["mkdir"]  = std::bind(&Terminal::mkdir,            this, std::placeholders::_1, std::placeholders::_2);
    _commands["mkfile"] = std::bind(&Terminal::mkfile,           this, std::placeholders::_1, std::placeholders::_2);
    _commands["sb"]     = std::bind(&Terminal::sb,               this, std::placeholders::_1, std::placeholders::_2);
    _commands["gi"]     = std::bind(&Terminal::get_inode,        this, std::placeholders::_1, std::placeholders::_2);
    _commands["gb"]     = std::bind(&Terminal::get_block,        this, std::placeholders::_1, std::placeholders::_2);
    _commands["chain"]  = std::bind(&Terminal::get_chain,        this, std::placeholders::_1, std::placeholders::_2);
    _commands["cd"]     = std::bind(&Terminal::change_directory, this, std::placeholders::_1, std::placeholders::_2);
    _commands["cat"]    = std::bind(&Terminal::cat,              this, std::placeholders::_1, std::placeholders::_2);
    _commands["wr"]     = std::bind(&Terminal::write,            this, std::placeholders::_1, std::placeholders::_2);
    _commands["wa"]     = std::bind(&Terminal::write_append,     this, std::placeholders::_1, std::placeholders::_2);
    _commands["rm"]     = std::bind(&Terminal::rm,               this, std::placeholders::_1, std::placeholders::_2);

    _commands.emplace("cls",        [](std::vector<std::string> args, Directory*) { system("cls"); });
    _commands.emplace("shutdown",   [](std::vector<std::string> args, Directory*) { std::cout << "Shutdowning..."; });
}

int Terminal::Listen()
{
    std::string line;

    std::cin.ignore(1);

    while (true) {
        std::cout << _file_system->current_user()->name() << "@eds " << path.ToString() << ": ";
        std::getline(std::cin, line);
        try
        {
            this->execute_command(line);
        }
        catch (const std::exception& e)
        {
            std::cout << "Executing error: " << e.what() << std::endl;
        }

        std::cout << std::endl;

        if (line == "shutdown") {
            return 0;
        }
    }

    return 0;
}

void Terminal::execute_command(const std::string& line) {

    if (line == "") {
        return;
    }

    auto tokens = split(line, ' ');

    std::string cmd = tokens[0];
    Directory* dir = _file_system->current_directory();
    if (tokens.size() > 1) {
        if (tokens[1].find('/') != std::string::npos) {
            dir = traverse_to_dir(tokens[1]);
            tokens[1] = Path::GetLastSegment(tokens[1]);
        }
    }
    if (_commands.find(cmd) != _commands.end()) {
        _commands[cmd](tokens, dir);
    }
    else {
        std::cout << "Unknown command" << std::endl;
    }
}

bool Terminal::find_arg(std::vector<std::string> args, const std::string& arg)
{
    return (std::find(args.begin(), args.end(), arg) != args.end());
}

void Terminal::mkfile(std::vector<std::string> args, Directory* dir)
{
    if (args.size() < 2) {
        throw execution_exception("Enter an args", "mkfile");
    }

    std::string name = args[1];
    
    if (name.length() == 0 && name.length() > 16) {
        std::cout << "invalid name (0 < n < 16)" << std::endl;
        return;
    }
    if (dir->exists(name)) {
        throw std::exception("Element with this name already exists");
    }
    
    _file_system->CreateFile(name, dir);
}
void Terminal::mkdir(std::vector<std::string> args, Directory* dir)
{
    if (args.size() < 1) {
        throw execution_exception("Enter an args. Try execute help mkdir", "mkdir");
    }

    std::string name = args[1];

    if (name.length() == 0 && name.length() > 16) {
        std::cout << "invalid name (0 < n < 16)" << std::endl;
        return;
    }

    if (dir->exists(name)) {
        throw std::exception("Element with this name already exists");
    }
    
    _file_system->CreateDirectory(name, dir);    
}
void Terminal::rm(std::vector<std::string> args, Directory* dir)
{    
    std::string name = args[1];

    if (name.length() == 0 && name.length() > 16) {
        std::cout << "invalid name (0 < n < 16)" << std::endl;
        return;
    }

    try
    {
        DEntry* dentry = dir->exists(name);
        INode* inode = dentry->inode();

        if (inode->IsDirectoryFlag()) {
            std::cout << "ERROR! cannot remove a directory" << std::endl;
            return;
        }

        _file_system->RemoveFile(dentry);

        delete dentry;
    }
    catch (const std::exception& e)
    {
        throw e;
    }
}
void Terminal::rmdir(std::vector<std::string> args, Directory* dir)
{
}
void Terminal::sb(std::vector<std::string> args, Directory* dir)
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
void Terminal::get_block(std::vector<std::string> args, Directory* dir)
{
    int id;
    if (args.size() == 1) {
        std::cout << "id: ";
        std::cin >> id;
    }
    else {
        try
        {
            id = std::stoi(args[1]);
        }
        catch (const std::exception& e)
        {
            std::cout << "ERROR! Enter an id of block to inspect" << std::endl;
            return;
        }
        
    }
   
    
    try
    {
        Block* block = _file_system->GetBlock(id);
        std::cout << "Block info: " << std::endl;
        std::cout << *block;
        delete block;
    }
    catch (const std::exception& e)
    {
        std::cout << "При выполнении произошла ошибка: " << e.what() << std::endl;
    }
}
void Terminal::get_inode(std::vector<std::string> args, Directory* dir)
{
    int id;
    if (args.size() == 1) {
        std::cout << "id: ";
        std::cin >> id;
    }
    else {
        try
        {
            id = std::stoi(args[1]);
        }
        catch (const std::exception& e)
        {
            std::cout << "ERROR! Enter an id of block to inspect" << std::endl;
            return;
        }

    }

    try
    {
        INode* inode = _file_system->GetInode(id);
        std::cout << "id\tflags\tmode\tsize\tcreation date\t\tname" << std::endl;
        std::cout << *inode << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "При выполнении произошла ошибка: " << e.what() << std::endl;
    }
}
void Terminal::get_chain(std::vector<std::string> args, Directory* dir)
{
    if (args.size() < 1) {
        throw execution_exception("Enter an args. Try execute help mkdir", "mkdir");
    }
    
    std::string name = args[1];

    auto dentry = dir->exists(name);

    if (!dentry) {
        std::cout << "ERROR! cannot find a file" << std::endl;
        return;
    }

    INode* inode = dentry->inode();

    auto chain = _file_system->services()->block_service()->GetBlockchain(inode);

    for (int el : chain) {
        std::cout << el;
        if (el != -2) {
            std::cout << " -> ";
        }       
    }
    std::cout << std::endl;
}
void Terminal::ls(std::vector<std::string> args, Directory* dir)
{
    auto vector = _file_system->ls();
    std::cout << "id\tflags\tmode\tsize\tcreation date\t\towner\tname" << std::endl;

    for (int i = 0; i < vector.size(); i++) {
        INode* inode = vector[i]->inode();
        User* user = _file_system->GetUser(inode->uid());
        std::cout << *inode << "\t" << user->name() << "\t" << vector[i]->name() << std::endl;

    }
}
void Terminal::change_directory(std::vector<std::string> args, Directory* dir)
{
    if (args.size() < 1) {
        throw execution_exception("Enter an args. Try execute help cd", "cd");
    }
    
    std::string name = args[1];

    if (name == "..") {
        try
        {
            _file_system->ChangeDirectory(_file_system->GetParentDirectory(dir));
            path = get_path();
            return;
        }
        catch (const std::exception& e)
        {
            throw e;
        }
    }
    else if (name == "") {
        _file_system->ChangeToRootDirectory();
        path = Path();
        path.add("");
        return;
    }

    try
    {
        DEntry* dentry = dir->exists(name);

        if (dentry == nullptr) {
            throw std::exception("Directory wasnt found");
        }

        INode* inode = dentry->inode();

        if (inode->IsDirectoryFlag() == false) {
            throw std::exception("ERROR! not a directory");
        }

        _file_system->ChangeDirectory((Directory*)dentry);

        path = get_path();

        return;
    }
    catch (const std::exception& e)
    {
        throw e;
    }
    throw std::exception("ERROR! cannot find a directory");
}
void Terminal::cat(std::vector<std::string> args, Directory* dir)
{
    if (args.size() == 1) {
        throw execution_exception("Enter an args. Try execute help cat", "cat");
    }

    std::string name = args[1];

    if (name.length() == 0 && name.length() > 16) {
        std::cout << "invalid name (0 < n < 16)" << std::endl;
        return;
    }

    try
    {
        DEntry* dentry = dir->exists(name);
        INode* inode = dentry->inode();

        if (inode->IsDirectoryFlag() == true) {
            throw std::exception("ERROR! cannot open a directory");
        }

        char* content = _file_system->ReadFile((File*)dentry);

        std::cout << content << std::endl;
        return;
    }
    catch (const std::exception& e)
    {
        throw e;
    }
    _file_system->CreateFile(name, dir);
}
void Terminal::write(std::vector<std::string> args, Directory* dir)
{
    if (args.size() < 1) {
        throw execution_exception("Enter an args. Try execute help cat", "cat");
    }
   
    std::string name = args[1];

    DEntry* dentry = nullptr;

    try
    {
        dentry = dir->exists(name);
    }
    catch (const std::exception& e)
    {
        mkfile(args, dir);
        dentry = dir->exists(name);
    } 

    INode* inode = dentry->inode();

    if (inode->IsDirectoryFlag()) {
        std::cout << "ERROR! cannot open a directory" << std::endl;
        return;
    }

    std::string text;
    for (std::string line; std::getline(std::cin, line); ) {
        if (line.at(0) == '\x4') {
            break; 
        }
        if (text == "") {
            text += line;
        }
        else {
            text += "\n" + line;
        }
    }
    text += '\0';

    _file_system->WriteFile((File*)dentry, text);
}
void Terminal::write_append(std::vector<std::string> args, Directory* dir)
{
    if (args.size() < 1) {
        throw execution_exception("Enter an args. Try execute help wa", "wa");
    }

    std::string name = args[1];

    DEntry* dentry = nullptr;

    try
    {
        dentry = dir->exists(name);
    }
    catch (const std::exception& e)
    {
        mkfile(args, dir);
        dentry = dir->exists(name);
    }

    INode* inode = dentry->inode();

    if (inode->IsDirectoryFlag()) {
        std::cout << "ERROR! cannot open a directory" << std::endl;
        return;
    }

    std::string text;
    for (std::string line; std::getline(std::cin, line); ) {
        if (line.at(0) == '\x4') {
            break;
        }
        if (text == "") {
            text += line;
        }
        else {
            text += "\n" + line;
        }
    }
    text += '\0';

    _file_system->AppendFile((File*)dentry, text);
}


void Terminal::move(std::vector<std::string> args, Directory* dir)
{
    throw std::exception("NOT IMPLEMENTED");

    if (args.size() < 2) {
        throw execution_exception("Enter an args. Try execute help wa", "wa");
    }

    Directory* source = dir;
    Directory* target = traverse_to_dir(args[2]);
    std::string source_name = Path::GetLastSegment(args[1]);
    std::string target_name = Path::GetLastSegment(args[2]);

    
    DEntry* source_dentry = source->exists(source_name);
    
    
    source->remove(source_dentry);

   
}

Directory* Terminal::traverse_to_dir(std::string path_string)
{
    auto path = Path(path_string).parts();

    Directory* current_directory = nullptr;
    if (path.size() == 1) {
        current_directory = _file_system->root_directory();
    }
    else if (path[0].compare("") == 0) {
        path.erase(path.begin());
        current_directory = _file_system->root_directory();
    }
    else {
        current_directory = _file_system->current_directory();
    }

    for (int i = 0; i < path.size() - 1; i++) {
        std::string dir_name = path[i];

        if (dir_name.compare("..") == 0) {
            current_directory = (Directory*) current_directory->parent();
        }
        else {
            auto dentry = current_directory->exists(dir_name);
            if (dentry != nullptr) {
                current_directory = _file_system->GetDirectory(dentry->inode(), current_directory);
            }
            else {
                current_directory = _file_system->CreateDirectory(dir_name, current_directory);
            }
        }
    }

    return current_directory;
}

Path Terminal::get_path()
{
    Path path = Path();
    bool fl = true;

    Directory* dir = _file_system->current_directory();

    while (dir != nullptr) {
        std::string name = dir->name();
        path.add(name);

        dir = (Directory*)dir->parent();
    }


    return path.reverse();
}
Path Terminal::get_path(Directory* directory)
{
    Path path = Path();
    bool fl = true;

    Directory* dir = directory;

    while (dir != nullptr) {
        std::string name = dir->name();
        path.add(name);

        dir = (Directory*)dir->parent();
    }
        

    return path.reverse();
}

execution_exception::execution_exception(const std::string& message, const std::string& command) : std::exception(message.c_str())
{
    _message = message;
    _command = command;
}
const char* execution_exception::what() noexcept
{
    return std::string(_command + ": " + _message).c_str();
}
