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

    _commands = std::map<std::string, std::function<void(std::vector<std::string> args)>>();

    _commands["ls"]     = std::bind(&Terminal::ls,               this, std::placeholders::_1);
    _commands["mkdir"]  = std::bind(&Terminal::mkdir,            this, std::placeholders::_1);
    _commands["mkfile"] = std::bind(&Terminal::mkfile,           this, std::placeholders::_1);
    _commands["sb"]     = std::bind(&Terminal::sb,               this, std::placeholders::_1);
    _commands["gi"]     = std::bind(&Terminal::get_inode,        this, std::placeholders::_1);
    _commands["gb"]     = std::bind(&Terminal::get_block,        this, std::placeholders::_1);
    _commands["chain"]  = std::bind(&Terminal::get_chain,        this, std::placeholders::_1);
    _commands["cd"]     = std::bind(&Terminal::change_directory, this, std::placeholders::_1);
    _commands["cat"]    = std::bind(&Terminal::cat,              this, std::placeholders::_1);
    _commands["wr"]     = std::bind(&Terminal::write,            this, std::placeholders::_1);
    _commands["wa"]     = std::bind(&Terminal::write_append,     this, std::placeholders::_1);
    _commands["rm"]     = std::bind(&Terminal::rm,               this, std::placeholders::_1);

    _commands.emplace("cls",        [](std::vector<std::string> args) { system("cls"); });
    _commands.emplace("shutdown",   [](std::vector<std::string> args) { std::cout << "Shutdowning..."; });
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

    if (_commands.find(cmd) != _commands.end()) {
        _commands[cmd](tokens);
    }
    else {
        std::cout << "Unknown command" << std::endl;
    }
    std::cout << std::endl;
}

bool Terminal::find_arg(std::vector<std::string> args, const std::string& arg)
{
    return (std::find(args.begin(), args.end(), arg) != args.end());
}

DEntry* Terminal::exists(std::string name)
{
    for (auto dentry : _file_system->current_directory()->dentry()) {
        if (name == dentry->name()) {
            return dentry;
        }
    }
    return nullptr;
}
DEntry* Terminal::exists(int inode_id)
{
    for (auto dentry : _file_system->current_directory()->dentry()) {
        if (inode_id == dentry->inode_id()) {
            return dentry;
        }
    }
    return nullptr;
}
DEntry* Terminal::exists(std::string name, Directory* dir)
{
    for (auto dentry : dir->dentry()) {
        if (name == dentry->name()) {
            return dentry;
        }
    }
    return nullptr;
}
DEntry* Terminal::exists(int inode_id, Directory* dir)
{
    for (auto dentry : dir->dentry()) {
        if (inode_id == dentry->inode_id()) {
            return dentry;
        }
    }
    return nullptr;
}

void Terminal::mkfile(std::vector<std::string> args)
{
    std::string name;
    Directory* optional_dir = nullptr;
    if (args.size() == 1) {
        std::cout << "\tfile name: ";
        std::cin >> name;
    }
    else {
        name = args[1];
    }
    
    if (name.find('/') != std::string::npos) {
        optional_dir = traverse_to_dir(name);
        name = Path::GetLastSegment(name);
    }

    if (name.length() == 0 && name.length() > 16) {
        std::cout << "invalid name (0 < n < 16)" << std::endl;
        return;
    }

    if (exists(name)) {
        std::cout << "ERROR! Element with this name already exists" << std::endl;
        return;
    }

    if (optional_dir) {
        _file_system->CreateFileAt(name, optional_dir);
    }
    else {
        _file_system->CreateFile(name);
    }
}
void Terminal::mkdir(std::vector<std::string> args)
{
    std::string name;
    Directory* optional_dir = nullptr;
    if (args.size() == 1) {
        std::cout << "\tdirectory name: ";
        std::cin >> name;
    }
    else {
        name = args[1];
    }

    if (name.find('/') != std::string::npos) {
        optional_dir = traverse_to_dir(name);
        name = Path::GetLastSegment(name);
    }

    if (name.length() == 0 && name.length() > 16) {
        std::cout << "invalid name (0 < n < 16)" << std::endl;
        return;
    }

    if (exists(name)) {
        std::cout << "ERROR! Element with this name already exists" << std::endl;
        return;
    }

    if (optional_dir) {
        _file_system->CreateDirectoryAt(name, optional_dir);
    }
    else {
        _file_system->CreateDirectory(name);
    }
    
}
void Terminal::rm(std::vector<std::string> args)
{
    std::string name;
    if (args.size() == 1) {
        std::cout << "\tfile name to open: ";
        std::cin >> name;
    }
    else {
        name = args[1];
    }

    if (name.length() == 0 && name.length() > 16) {
        std::cout << "invalid name (0 < n < 16)" << std::endl;
        return;
    }

    for (auto dentry : _file_system->current_directory()->dentry()) {
        if (name == dentry->name()) {
            INode* inode = _file_system->GetInode(dentry->inode_id());

            if (inode->IsDirectoryFlag() == true) {
                std::cout << "ERROR! cannot remove a directory" << std::endl;
                return;
            }

            _file_system->RemoveFile(inode->id());
        }
    }
}
void Terminal::rmdir(std::vector<std::string> args)
{
}
void Terminal::sb(std::vector<std::string> args)
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
void Terminal::get_block(std::vector<std::string> args)
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
void Terminal::get_inode(std::vector<std::string> args)
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
void Terminal::get_chain(std::vector<std::string> args)
{
    std::string name;
    if (args.size() == 1) {
        std::cout << "\tfile name to open: ";
        std::cin >> name;
    }
    else {
        name = args[1];
    }

    auto dentry = exists(name);

    if (!dentry) {
        std::cout << "ERROR! cannot find a file" << std::endl;
        return;
    }

    INode* inode = _file_system->services()->inode_service()->Get(dentry->inode_id());

    auto chain = _file_system->services()->block_service()->GetBlockchain(inode);

    for (int el : chain) {
        std::cout << el;
        if (el != -2) {
            std::cout << " -> ";
        }       
    }
    std::cout << std::endl;
}
void Terminal::ls(std::vector<std::string> args)
{
    auto vector = _file_system->ls();
    std::cout << "id\tflags\tmode\tsize\tcreation date\t\towner\tname" << std::endl;

    for (int i = 0; i < vector.size(); i++) {
        INode* inode = _file_system->GetInode(vector[i]->inode_id());
        User* user = _file_system->GetUser(inode->uid());
        std::cout << *inode << "\t" << user->name() << "\t" << vector[i]->name() << std::endl;

    }
}
void Terminal::change_directory(std::vector<std::string> args)
{

    std::string name;
    Directory* dir = _file_system->current_directory();
    if (args.size() == 1) {
        std::cout << "directory name to change: ";
        std::cin >> name;
    }
    else {
        name = args[1];
    }

    if (name.find('/') != std::string::npos && name != "/") {
        dir = traverse_to_dir(name);
        name = Path::GetLastSegment(name);
    }

    if (name == "..") {
        try
        {
            dir = _file_system->GetParentDirectory(dir);
            _file_system->ChangeDirectory(dir);
            path = get_path(_file_system->current_directory());
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
        
        return;
    }
    else if (name == "/") {
        _file_system->ChangeToRootDirectory();
        path = Path();
        path.add("");
        return;
    }

    for (auto dentry : dir->dentry()) {
        if (name == dentry->name()) {
            INode* inode = _file_system->GetInode(dentry->inode_id());

            if (inode->IsDirectoryFlag() == false) {
                std::cout << "ERROR! not a directory" << std::endl;
                return;
            }

            Directory* d = _file_system->GetDirectory(inode->id());
            _file_system->ChangeDirectory(d);
            path = get_path(_file_system->current_directory());

            delete inode;
            return;
        }
    }
    std::cout << "ERROR! cannot find a directory" << std::endl;
}
void Terminal::cat(std::vector<std::string> args)
{
    std::string name;
    if (args.size() == 1) {
        std::cout << "\tfile name to open: ";
        std::cin >> name;
    }
    else {
        name = args[1];
    }

    if (name.length() == 0 && name.length() > 16) {
        std::cout << "invalid name (0 < n < 16)" << std::endl;
        return;
    }

    for (auto dentry : _file_system->current_directory()->dentry()) {
        if (name == dentry->name()) {
            INode* inode = _file_system->GetInode(dentry->inode_id());

            if (inode->IsDirectoryFlag() == true) {
                std::cout << "ERROR! cannot open a directory" << std::endl;
                return;
            }

            char* content = _file_system->ReadFile(inode->id());

            std::cout << content << std::endl;
            return;
        }
    }
    _file_system->CreateFile(name);
}

void Terminal::write(std::vector<std::string> args)
{
    std::string name;
    if (args.size() == 1) {
        std::cout << "\tfile name to open: ";
        std::cin >> name;
    }
    else {
        name = args[1];
    }

    auto dentry = exists(name);

   

    if (!dentry) {
        mkfile(args);
        dentry = exists(name);
    }

    INode* inode = _file_system->GetInode(dentry->inode_id());

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

    _file_system->WriteFile(inode->id(), text);
}
void Terminal::write_append(std::vector<std::string> args)
{
    std::string name;
    if (args.size() == 1) {
        std::cout << "\tfile name to open: ";
        std::cin >> name;
    }
    else {
        name = args[1];
    }

    auto dentry = exists(name);



    if (!dentry) {
        mkfile(args);
        dentry = exists(name);
    }

    INode* inode = _file_system->GetInode(dentry->inode_id());

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

    _file_system->AppendFile(inode->id(), text);
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
            current_directory = _file_system->GetParentDirectory(current_directory);
        }
        else {
            auto dentry = exists(dir_name, current_directory);
            if (dentry != nullptr) {
                current_directory = _file_system->GetDirectory(dentry->inode_id());
            }
            else {
                current_directory = _file_system->CreateDirectoryAt(dir_name, current_directory);
            }
        }
    }

    return current_directory;
}

Path Terminal::get_path(Directory* directory)
{
    Path path = Path();
    bool fl = true;
    int inode_id = directory->inode_id();

    if (directory->parent() != -1) {
        Directory* dir = _file_system->GetParentDirectory(directory);

        while (fl) {
            fl = !(dir->parent() == -1);
            std::string name = exists(inode_id, dir)->name();
            path.add(name);
            inode_id = dir->inode_id();
            if (fl) {
                dir = _file_system->GetParentDirectory(dir);
            }
        }
    }
    path.add("");

    return path.reverse();
}
