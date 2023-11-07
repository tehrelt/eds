#include "dentry_factory.h"
#include "file.h"
#include "directory.h"

DEntry* DEntryFactory::CREATE(int type, INode* inode, DEntry* parent, const std::string& name)
{
	if (type == FILE) {
		return new File(inode, parent, name);
	}
	else if (type == DIRECTORY) {
		Directory* dir = new Directory(inode, parent, name);
		dir->init();
		return dir;
	}
	throw std::exception("INVALID DENTRY_TYPE");
}
