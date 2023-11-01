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

std::string Path::ToString()
{
    std::string path;

    for (int i = 0; i < _parts.size(); i++) {
        path += _parts[i] + "/";
    }

    return path;
}
