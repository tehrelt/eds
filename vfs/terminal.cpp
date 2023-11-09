#include "terminal.h"
#include "tools.h"
#include <sstream>


Terminal::Terminal(FileSystem* fs)
{
    _fs = fs;

    _commands = std::map<std::string, std::function<void(std::vector<std::string>, Directory*)>>();

    _commands["ls"]     = std::bind(&Terminal::ls,               this, std::placeholders::_1, std::placeholders::_2);
    _commands["mkdir"]  = std::bind(&Terminal::mkdir,            this, std::placeholders::_1, std::placeholders::_2);
    _commands["mkfile"] = std::bind(&Terminal::mkfile,           this, std::placeholders::_1, std::placeholders::_2);
    _commands["sb"]     = std::bind(&Terminal::sb,               this, std::placeholders::_1, std::placeholders::_2);
  //_commands["chain"]  = std::bind(&Terminal::get_chain,        this, std::placeholders::_1, std::placeholders::_2);
    _commands["cd"]     = std::bind(&Terminal::change_directory, this, std::placeholders::_1, std::placeholders::_2);
    _commands["cat"]    = std::bind(&Terminal::cat,              this, std::placeholders::_1, std::placeholders::_2);
    _commands["wr"]     = std::bind(&Terminal::write,            this, std::placeholders::_1, std::placeholders::_2);
    _commands["wa"]     = std::bind(&Terminal::write_append,     this, std::placeholders::_1, std::placeholders::_2);
    _commands["rm"]     = std::bind(&Terminal::rm,               this, std::placeholders::_1, std::placeholders::_2);
    _commands["mv"]     = std::bind(&Terminal::move,             this, std::placeholders::_1, std::placeholders::_2);
    _commands["su"]     = std::bind(&Terminal::switch_user,      this, std::placeholders::_1, std::placeholders::_2);
    _commands["cu"]     = std::bind(&Terminal::create_user,      this, std::placeholders::_1, std::placeholders::_2);
    _commands["users"]  = std::bind(&Terminal::users,            this, std::placeholders::_1, std::placeholders::_2);
    _commands["who"]    = std::bind(&Terminal::who,              this, std::placeholders::_1, std::placeholders::_2);
    _commands["chmod"]  = std::bind(&Terminal::chmod, this, std::placeholders::_1, std::placeholders::_2);

    _commands.emplace("cls", [](std::vector<std::string> args, Directory*) { system("cls"); });
    _commands.emplace("shutdown", [](std::vector<std::string> args, Directory*) { std::cout << "Shutdowning..."; });

}

int Terminal::Listen()
{
    std::string line;

    std::cin.ignore(1);

    while (true) {
        std::cout << _fs->current_user()->name() << "@eds " << _fs->current_directory()->path()->ToString() << ": ";
        std::getline(std::cin, line);

        line = trim(line);

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
    Directory* dir =  _fs->current_directory();
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
    
    dir->createFile(name, _fs->current_user()->id());
}
void Terminal::mkdir(std::vector<std::string> args, Directory* dir)
{
    if (args.size() < 2) {
        throw execution_exception("Enter an args. Try execute help mkdir", "mkdir");
    }

    std::string name = args[1];

    if (name.length() == 0 && name.length() > 16) {
        std::cout << "invalid name (0 < n < 16)" << std::endl;
        return;
    }
    
    dir->createDirectory(name, _fs->current_user()->id());
}
void Terminal::rm(std::vector<std::string> args, Directory* dir)
{    
    std::string name = args[1];

    if (name.length() == 0 && name.length() > 16) {
        std::cout << "invalid name (0 < n < 16)" << std::endl;
        return;
    }

    dir->removeFile(name);
}
void Terminal::sb(std::vector<std::string> args, Directory* dir)
{
    Superblock* sb =  _fs->sb();

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
void Terminal::ls(std::vector<std::string> args, Directory* dir)
{
    auto dentries =  _fs->current_directory()->dentries();
    std::cout << "id\tflags\tmode\tsize\tcreation\t\tmodify\t\t\tlast access\t\towner\tname" << std::endl;

    for (int i = 0; i < dentries.size(); i++) {
        INode* inode = dentries[i]->inode();
        User* user =  _fs->getUserById(inode->uid());
        std::cout << *inode << "\t" << user->name() << "\t" << dentries[i]->name() << std::endl;

    }
}
void Terminal::change_directory(std::vector<std::string> args, Directory* dir)
{
    if (args.size() < 2) {
        throw execution_exception("Enter an args. Try execute help cd", "cd");
    }
    
    std::string name = args[1];

    if (name == "") {
         _fs->forwardTo(_fs->root_directory());
        return;
    }

    try
    {
        Directory* d = dir->getDirectory(name);

        if (d == nullptr) {
            throw std::exception("Directory wasnt found");
        }

         _fs->forwardTo(d);
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
    if (args.size() < 2) {
        throw execution_exception("Enter an args. Try execute help cat", "cat");
    }

    std::string name = args[1];

    if (name.length() == 0 && name.length() > 16) {
        std::cout << "invalid name (0 < n < 16)" << std::endl;
        return;
    }

    try
    {
        File* dentry = dir->getFile(name);
        INode* inode = dentry->inode();

        if (inode->IsDirectoryFlag() == true) {
            throw std::exception("ERROR! cannot open a directory");
        }

        if (!inode->is_r_____()) {
            throw std::exception("Permission denied");
        }
        else if (!inode->is____r__() && _fs->current_user()->id() != inode->uid()) {
            throw std::exception("Permission denied");
        }

        char* content =  dentry->read();

        std::cout << content << std::endl;
        return;
    }
    catch (const std::exception& e)
    {
        throw e;
    }
     dir->createFile(name, _fs->current_user()->id());
}
void Terminal::write(std::vector<std::string> args, Directory* dir)
{
    if (args.size() < 2) {
        throw execution_exception("Enter an args. Try execute help cat", "cat");
    }
   
    std::string name = args[1];

    File* file = nullptr;
    try
    {
        file = dir->getFile(name);
    }
    catch (const no_such_file_exception&)
    {
        mkfile(args, dir);
        file = dir->getFile(name);
    }
    
    INode* inode = file->inode();

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

    file->write(text.c_str(), text.size());
}
void Terminal::write_append(std::vector<std::string> args, Directory* dir)
{
    if (args.size() < 2) {
        throw execution_exception("Enter an args. Try execute help wa", "wa");
    }

    std::string name = args[1];

    File* file = nullptr;
    try
    {
        file = dir->getFile(name);
    }
    catch (const no_such_file_exception&)
    {
        mkfile(args, dir);
        file = dir->getFile(name);
    }

    INode* inode = file->inode();

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

    file->write(text.c_str(), file->length() - 1, text.size());
}
void Terminal::switch_user(std::vector<std::string> args, Directory* dir)
{
    std::string username;
    std::string password;
    if (args.size() == 1) {
        username = "root";
    }
    else {
        username = args[1];
    }

    if (_fs->userExists(username) == false) {
        throw std::exception(std::string("user " + username + " not found").c_str());
    }

    std::cout << "SWITCHING TO " << username << std::endl;
    std::cout << "ENTER A PASSWORD: "; std::cin >> password;

    if (!_fs->Login(username, password)) {
        throw std::exception("invalid credentials");
    }
    std::cout << "Successfully login" << std::endl;
}
void Terminal::create_user(std::vector<std::string> args, Directory* dir)
{
    std::string username;
    std::string password;

    std::cout << "ENTER A USERNAME: "; std::cin >> username;

    if (username.size() > 10) {
        throw std::exception("username is too long");
    }

    std::cout << "ENTER A PASSWORD: "; std::cin >> password;

    _fs->CreateUser(username, password);
}
void Terminal::users(std::vector<std::string> args, Directory* dir)
{
    int users_count = Storage::STORAGE()->getNextUID();

    std::cout << "List of users: " << std::endl;
    for (int i = 0; i < users_count; i++) {
        std::cout << _fs->getUserById(i)->name() << std::endl;
    }
}
void Terminal::who(std::vector<std::string> args, Directory* dir)
{
    std::cout << _fs->current_user()->name() << std::endl;
}
void Terminal::chmod(std::vector<std::string> args, Directory* dir)
{
    if (args.size() < 3) {
        throw execution_exception("Enter an args. Try execute help chmod", "chmod");
    }

    if (args[2].size() < 2) {
        throw execution_exception("Mode must contain exact two numbers.", "chmod");
    }

    std::string name = args[1];
    int mode = std::stoi(args[2]);

    if (mode > 77) {
        throw execution_exception("Mode is too big. Each number must be in range 0 to 7", "chmod");
    }

    int owner = mode / 10;
    int others = mode % 10;

    mode = 0;
    
    DEntry* dentry = dir->findByName(name);
    INode* inode = dentry->inode();

    if (!_fs->checkUser(inode)) {
        throw execution_exception("Permission denied", "chmod");
    }

    for (int i = 0; i < 6; i++) {
        if (i < 3) {
            mode |= (others & (1 << i));
        }
        else {
            mode |= ((owner & (1 << (i-3))) << 3);
        }
    }
    
    dentry->set_mode(mode);

    Storage::STORAGE()->saveINode(inode);
}

void Terminal::move(std::vector<std::string> args, Directory* dir)
{
    if (args.size() < 3) {
        throw execution_exception("Enter an args. Try execute help mv", "wa");
    }


    Directory* source = dir;
    Directory* target = traverse_to_dir(args[2]);
    std::string source_name = Path::GetLastSegment(args[1]);

    if (args[2][args[2].size() - 1] == '/') {
        throw std::exception("TARGET DESTINATION must include file name as last segment of path");
    }

    std::string target_name = Path::GetLastSegment(args[2]);

    
    DEntry* source_dentry = source->findByName(source_name);
    source_dentry->set_name(target_name);

    source->moveTo(source_dentry, target);
}

Directory* Terminal::traverse_to_dir(std::string path_string)
{
    auto path = Path(path_string);
    auto parts = path.parts();

    int sp = path.is_absolute() ? 1 : 0;
    Directory* current_directory  = path.is_absolute() ? _fs->root_directory() : _fs->current_directory();


    for (int i = sp; i < parts.size() - 1; i++) {
        std::string dir_name = parts[i];

        if (dir_name.compare("..") == 0) {
            if (current_directory->parent() == nullptr) {
                throw std::exception("CANNOT GET BEYOND ROOT DIRECTORY");
            }
            current_directory = (Directory*) current_directory->parent();
        }
        else {
            if (current_directory->exists(dir_name)) {
                current_directory = current_directory->getDirectory(dir_name);
            }
            else {
                current_directory = current_directory->createDirectory(dir_name, _fs->current_user()->id());
            }
        }
    }

    return current_directory;
}

Path Terminal::get_path()
{
    Path path = Path();
    bool fl = true;

    Directory* dir =  _fs->current_directory();

    while (dir != nullptr) {
        std::string name = dir->name();
        path.push(name);

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
        path.push(name);

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

//void Terminal::get_chain(std::vector<std::string> args, Directory* dir)
//{
//    if (args.size() < 1) {
//        throw execution_exception("Enter an args. Try execute help mkdir", "mkdir");
//    }
//    
//    std::string name = args[1];
//
//    auto dentry = dir->exists(name);
//
//    if (!dentry) {
//        std::cout << "ERROR! cannot find a file" << std::endl;
//        return;
//    }
//
//    INode* inode = dentry->inode();
//
//    auto chain =  _fs->services()->block_service()->GetBlockchain(inode);
//
//    for (int el : chain) {
//        std::cout << el;
//        if (el != -2) {
//            std::cout << " -> ";
//        }       
//    }
//    std::cout << std::endl;
//}