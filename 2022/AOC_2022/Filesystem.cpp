#include "Filesystem.h"

#include <string>

using namespace std;

Filesystem::Filesystem() 
    :dirstack{}
, root(Directory{ "/" })
{
    cd("/");
}

Filesystem::Directory& Filesystem::cd(const string& name) {
    if (name == "..") {
        dirstack.pop();
    }
    else if (name == "/") {
        while (!dirstack.empty()) {
            dirstack.pop();
        }
        dirstack.push(&root);
    }
    else {
        auto subdirIter = ranges::find_if(dirstack.top()->contents, [&name](const auto& d) { return d->name == name; });
        auto subdirPrt = static_cast<Directory*>(subdirIter->get());
        dirstack.push(subdirPrt);
    }

    return *dirstack.top();
}

void Filesystem::addFile(const string& name, size_t size) {
    dirstack.top()->contents.push_back(make_unique<File>(name, size));
}

void Filesystem::addDir(const string& name) {
    dirstack.top()->contents.push_back(make_unique<Directory>(name));
}

void Filesystem::accept(Filesystem::Visitor& v) {
    dirstack.top()->accept(v);
}