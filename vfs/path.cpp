#include "path.h"
#include "tools.h"

Path::Path()
{ 
    _parts = std::vector<std::string>(); 
}
Path::Path(std::string path)
{
    _parts = split(path, '/');
}
void Path::add(std::string part)
{
    _parts.push_back(part);
}

void Path::remove()
{
    _parts.pop_back();
}
std::string Path::last()
{
    return _parts.back();
}
Path& Path::operator+(const std::string& name)
{
    this->add(name);
    return *this;
}

std::string Path::ToString()
{
    std::string path;

    for (int i = 0; i < _parts.size(); i++) {
        path += _parts[i] + "/";
    }

    return path;
}

std::string Path::GetLastSegment(std::string path)
{
    return Path(path)._parts.back();
}
