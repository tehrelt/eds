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
Path::Path(std::vector<std::string> parts)
{
    _parts = parts;
}
void Path::push(std::string part)
{
    _parts.push_back(part);
}
void Path::pop()
{
    _parts.pop_back();
}

void Path::remove()
{
    _parts.pop_back();
}
Path Path::reverse()
{
    Path p = Path();
    for (int i = _parts.size() - 1; i >= 0; i--) {
        p.push(_parts[i]);
    }
    return p;
}
std::string Path::last()
{
    return _parts.back();
}
Path& Path::operator+(const std::string& name)
{
    this->push(name);
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
