#include "file.h"

File::File(INode* inode, DEntry* parent, const std::string& name)
	: DEntry(inode, parent, name)
{
	position = 0;
}
