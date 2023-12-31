#include "terminal.h"
#include "tools.h"
#include <sstream>
#include <iomanip>


Terminal::Terminal(FileSystem* fs)
{
    _fs = fs;

    _commands = std::map<std::string, std::function<void(std::vector<std::string>, Directory*)>>();

    _commands["ls"]     = std::bind(&Terminal::ls,               this, std::placeholders::_1, std::placeholders::_2);
    _commands["mkdir"]  = std::bind(&Terminal::mkdir,            this, std::placeholders::_1, std::placeholders::_2);
    _commands["mkfile"] = std::bind(&Terminal::mkfile,           this, std::placeholders::_1, std::placeholders::_2);
    _commands["sb"]     = std::bind(&Terminal::sb,               this, std::placeholders::_1, std::placeholders::_2);
    _commands["cd"]     = std::bind(&Terminal::change_directory, this, std::placeholders::_1, std::placeholders::_2);
    _commands["cat"]    = std::bind(&Terminal::cat,              this, std::placeholders::_1, std::placeholders::_2);
    _commands["wr"]     = std::bind(&Terminal::write,            this, std::placeholders::_1, std::placeholders::_2);
    _commands["wa"]     = std::bind(&Terminal::write_append,     this, std::placeholders::_1, std::placeholders::_2);
    _commands["rm"]     = std::bind(&Terminal::rm,               this, std::placeholders::_1, std::placeholders::_2);
    _commands["rmdir"]  = std::bind(&Terminal::rmdir,            this, std::placeholders::_1, std::placeholders::_2);
    _commands["mv"]     = std::bind(&Terminal::move,             this, std::placeholders::_1, std::placeholders::_2);
    _commands["su"]     = std::bind(&Terminal::switch_user,      this, std::placeholders::_1, std::placeholders::_2);
    _commands["cu"]     = std::bind(&Terminal::create_user,      this, std::placeholders::_1, std::placeholders::_2);
    _commands["cp"]     = std::bind(&Terminal::cp,               this, std::placeholders::_1, std::placeholders::_2);
    _commands["users"]  = std::bind(&Terminal::users,            this, std::placeholders::_1, std::placeholders::_2);
    _commands["who"]    = std::bind(&Terminal::who,              this, std::placeholders::_1, std::placeholders::_2);
    _commands["chmod"]  = std::bind(&Terminal::chmod,            this, std::placeholders::_1, std::placeholders::_2);
    _commands["tree"]   = std::bind(&Terminal::tree,             this, std::placeholders::_1, std::placeholders::_2);
    _commands.emplace("cls", [](std::vector<std::string> args, Directory*) { system("cls"); });
    _commands.emplace("shutdown", [](std::vector<std::string> args, Directory*) { std::cout << "Shutdowning..."; });

}

int Terminal::Listen()
{
    Log log = Log("Terminal::Listen");
    std::string line;

    std::cin.ignore(1);
   

    while (true) {
        std::cout << _fs->current_user()->name() << "@eds " << _fs->current_directory() << ": ";
        std::getline(std::cin, line);

        line = trim(line);

        try
        {
            this->execute_command(line);
        }
        catch (const execution_exception& e) {
            log.warn("cmd: " + e.command + "; error: " + e.message);
            std::cout << _guide[e.command] << std::endl;
        }
        catch (const std::exception& e)
        {
            log.warn("EXECUTION ERROR: " + std::string(e.what()));
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
        throw std::exception("invalid name (0 < n < 16)");
    } else if (isForbiddenName(name)) {
        throw std::exception("forbidden name");
    } else if (dir->exists(name)) {
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
    } else if (isForbiddenName(name)) {
        throw std::exception("forbidden name");
    } else if (dir->exists(name)) {
        throw std::exception("Element with this name already exists");
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

    File* file = dir->getFile(name);

    if (file->inode()->IsSystemFlag()) {
        throw execution_exception("cannot delete a system components", "rm");
    }

    if (!_fs->checkOwner(file->inode())) {
        throw execution_exception("you are not a owner or root", "rm");
    }

    dir->removeFile(name, _fs->current_user()->id());
}
void Terminal::rmdir(std::vector<std::string> args, Directory* dir)
{
    std::string name = args[1];

    if (name.length() == 0 && name.length() > 16) {
        std::cout << "invalid name (0 < n < 16)" << std::endl;
        return;
    }

    Directory* d = dir->getDirectory(name);

    if (d->inode()->IsSystemFlag()) {
        throw execution_exception("cannot delete a system components", "rm");
    }

    if (!_fs->checkOwner(dir->getDirectory(name)->inode())) {
        throw execution_exception("you are not a owner or root", "rm");
    }

    dir->removeDirectory(name, _fs->current_user()->id());
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
    Directory* directory = dir;

    if (args.size() > 1) {
        directory = directory->getDirectory(args[1]);
    }

    auto dentries = directory->dentries();

    std::cout << "id\tflags\tmode\tsize\tcreation\t\tmodify\t\t\tlast access\t\tbn\towner\tname" << std::endl;

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
        File* file = dir->getFile(name);
        INode* inode = file->inode();

        if (!inode->is_r_____()) {
            throw std::exception("Permission denied");
        }
        else if (!inode->is____r__() && (_fs->current_user()->id() != inode->uid() && _fs->current_user()->id() != 0)) {
            throw std::exception("Permission denied");
        }

        char* content =  file->read();

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
        throw execution_exception("Enter an args. Try execute help wr", "wr");
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

    if (!inode->is__w____()) {
        throw execution_exception("Permission denied", "wr");
    }
    else if (!inode->is_____w_() && (_fs->current_user()->id() != inode->uid() && _fs->current_user()->id() != 0)) {
        throw execution_exception("Permission denied", "wr");
    }
    else if (inode->IsSystemFlag()) {
        throw execution_exception("Cannot modify system files", "wr");
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

    file->seek(0, 0);
    file->clear();
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
        throw execution_exception("Cannot open a directory", "wa");
    }

    if (!inode->is__w____()) {
        throw execution_exception("Permission denied", "wa");
    }
    else if (!inode->is_____w_() && (_fs->current_user()->id() != inode->uid() && _fs->current_user()->id() != 0)) {
        throw execution_exception("Permission denied", "wa");
    }
    else if (inode->IsSystemFlag()) {
        throw execution_exception("Cannot modify system files", "wr");
    }

    bool first_line = true;

    std::string text;
    for (std::string line; std::getline(std::cin, line); ) {
        if (line.size() == 0) {
            text += "\n";
            continue;
        }
        else if (line.at(0) == '\x4') {
            break;
        }

        if (first_line) {
            text += line;
            first_line = false;
        }
        else {
            text += "\n" + line;
        }
    }
    text += '\0';

    
    file->seek(file->length() - 1, 0);
    file->write(text.c_str(), text.size());
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

    if (!_fs->login(username, password)) {
        throw std::exception("invalid credentials");
    }
    std::cout << "Successfully login" << std::endl;

    std::cin.ignore(1);
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

    _fs->createUser(username, password);

    std::cin.ignore(1);
}
void Terminal::users(std::vector<std::string> args, Directory* dir)
{
    int users_count = storage->getNextUID();

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
        throw execution_exception("", "chmod");
    }

    if (args[2].size() < 2) {
        throw execution_exception("Mode must contain exact two numbers.", "chmod");
    }

    std::string name; 
    int mode; 

    name = args[1];
    try
    {
        mode = std::stoi(args[2]);
    }
    catch (const std::exception&)
    {
        throw execution_exception("", "chmod");
    }
   

    if (mode > 77) {
        throw execution_exception("Mode is too big. Each number must be in range 0 to 7", "chmod");
    }

    int owner = mode / 10;
    int others = mode % 10;

    mode = 0;
    
    DEntry* dentry = dir->findByName(name);
    INode* inode = dentry->inode();

    if (!_fs->checkOwner(inode)) {
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

    storage->saveINode(inode);
}
void Terminal::move(std::vector<std::string> args, Directory* dir)
{
    if (args.size() < 3) {
        throw execution_exception("Enter an args. Try execute help mv", "wa");
    }

    Directory* source = dir;
    Directory* target = traverse_to_dir(args[2]);
    INode* target_inode = target->inode();

    if (args[2][args[2].size() - 1] == '/') {
        throw std::exception("TARGET DESTINATION must include file name as last segment of path");
    }
    else if (target_inode->uid() != _fs->current_user()->id() && !target_inode->is_____w_() && _fs->current_user()->id() != 0) {
        throw std::exception("unable move a dentry because you aren't owner of dest directory");
    }

    std::string source_name = Path::GetLastSegment(args[1]);
    std::string target_name = Path::GetLastSegment(args[2]);

    Path target_path = *target->path();
    target_path.push(target_name);

    DEntry* source_dentry = source->findByName(source_name);

    if (target_path.isPart(*source_dentry->path())) {
        throw std::exception("unable move a part of path upper");
    }

    

    if (!_fs->checkOwner(source_dentry->inode())) {
        throw std::exception("unable move a dentry because you aren't owner");
    }

    source_dentry->set_name(target_name);
    source->moveTo(source_dentry, target, _fs->current_user()->id());
}
void Terminal::cp(std::vector<std::string> args, Directory* dir)
{
    if (args.size() < 3) {
        throw execution_exception("Enter an args. Try execute help �p", "cp");
    }

    Directory* source = dir;
    Directory* target = traverse_to_dir(args[2]);

    std::string source_name = Path::GetLastSegment(args[1]);
    std::string target_name = Path::GetLastSegment(args[2]);

    if (target->exists(target_name)) {
        DEntry* dentry = target->findByName(target_name);
        if (dentry->getType() == FILE) {
            throw execution_exception("target already exists", "cp");
        }
    } 
    
    if (target_name[target_name.size() - 1] == '/') {
        throw std::exception("TARGET DESTINATION must include file name as last segment of path");
    }

    int type = source->findByName(source_name)->getType();

    if (type == DIRECTORY) {
        Directory* source_dir = source->getDirectory(source_name);
        Directory* target_dir = nullptr;

        try
        {
            target_dir = target->getDirectory(target_name);
        }
        catch (const std::exception&)
        {
            if (source_dir->inode()->is____r__()) {
                target_dir = target->createDirectory(target_name, _fs->current_user()->id());
            }
            else {
                throw execution_exception("cannot copy a directory; permission denied", "cp");
            }
            
        }
        
        source_dir->copyTo(target_dir, _fs->current_user()->id());
    }
    else {
        File* source_file = source->getFile(source_name);
        source->copyTo(source_file, target, target_name, _fs->current_user()->id());
    }

    //
    // 0. cp dir1 dir2     dir1/ 
    //                      | - subdir/
    //                      |      |  - subfile
    //                      | - file
    //                      | - file2
    // 1. ������ ����� dir2
    // 2. ������� � ������ ����� �������� ����� (dir1/)
    // 3. ������� ������ ���������� �� ����� �� dir1
    // 4. �������� ������ ����� �� ������ � dir2, �������� ����������
    // 5. ����� ����� ����������� �������� � ������ ������������� � �������� � ������ 1.
    // 6. ���� �� ������ ������ ����������, �� ��
    //
}
void Terminal::tree(std::vector<std::string> args, Directory* dir)
{
    Directory* directory = dir;

    if (args.size() > 1) {
        directory = directory->getDirectory(args[1]);
    }

    std::cout << directory->name() << std::endl;
    _tree(directory, 1);
}

#define indentX 2
#define prevLines for (int i = 0; i < indent; i++) { \
                    std::cout << "|" << std::setw(indentX); \
                  }
void Terminal::_tree(Directory* directory, int indent)
{
    auto dentires = directory->dentries();

    for (int i = directory->inode()->id() == 0 ? 1 : 2; i < dentires.size(); i++) {
        
        auto dentry = dentires[i];
        auto name = dentry->name();

        prevLines;

        std::cout << "---" << name << std::endl;

        if (dentry->getType() == DIRECTORY) {
            Directory* dir = directory->getDirectory(name);
            _tree(dir, indent + 1);
        }
    }
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
            if (current_directory->parent() != nullptr) {
                current_directory = (Directory*)current_directory->parent();
            }
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

execution_exception::execution_exception(const std::string& message, const std::string& command) : std::exception(message.c_str())
{
    this->message = message;
    this->command = command;
}
const char* execution_exception::what() noexcept
{
    return std::string(command + ": " + message).c_str();
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