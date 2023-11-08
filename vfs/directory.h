#pragma once
#include "file.h"
#include "dentry.h"
#include <vector>
#include "path.h"

#define DIRECTORY_ENTRY_SIZE sizeof(int) + 12
#define DIRECTORY_RECORD_CHAR_SIZE 12 + sizeof(int) + (DIRECTORY_ENTRY_SIZE)*_dentries.size()

class no_such_file_exception : public std::exception
{
public:
	std::string filename;
	std::string directory;

	no_such_file_exception(const std::string& filename, const std::string& directory) : std::exception("no such file") {
		this->directory = directory;
		this->filename = filename;
	}

	const char* what() noexcept {
		return std::string("no such" + filename + " at " + directory).c_str();
	}
};

class Directory : public DEntry
{
	std::vector<DEntry*>	_dentries;

	void save();
	void remove(DEntry* dentry);
	
	
	static Directory* parse(INode* inode, char* content, Directory* parent);

public:
	Directory(INode*);
	Directory(INode* inode, DEntry* parent, const std::string& name);

	void init();
	void add(DEntry* dentry);

	std::vector<DEntry*> dentries()	{ return _dentries; }

	bool exists(const std::string& name);

	DEntry* findByName(const std::string& name);

	File* createFile(std::string name, int uid);
	File* getFile(std::string name);
	void removeFile(std::string name);
	void moveTo(DEntry* dentry, Directory* to);

	Directory* createDirectory(std::string name, int uid);
	Directory* getDirectory(std::string name);
	Directory* removeDirectory(std::string name);

	char* to_char();

	int getType() override { return DIRECTORY; }

	static Directory* CREATE_ROOT();
	static Directory* READ_ROOT();
	static Directory* READ(DEntry*);
};

