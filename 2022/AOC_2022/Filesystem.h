#ifndef Filesystem_h_
#define Filesystem_h_

#include <string>
#include <vector>
#include <memory>
#include <stack>

class Filesystem {
public:
    //forward declare classes
    class File;
    class Directory;

    class Visitor {
    public:
        virtual void visit(File&) = 0;
        virtual void visit(Directory&) = 0;
    };

    class Entry {
    public:
        std::string name;
        Entry(std::string name)
            :name(name)
        {
        }
        virtual ~Entry() = default;
        virtual void accept(Visitor&) = 0;
    };

    class File : public Entry {
    public:
        size_t size;

        File(std::string name, size_t size)
            :Entry(name)
            , size(size)
        {}

        virtual ~File() = default;
        virtual void accept(Visitor& v) {
            v.visit(*this);
        }
    };

    class Directory : public Entry {
    public:
        std::vector<std::unique_ptr<Entry>> contents;
        Directory(std::string name)
            :Entry(name)
            , contents{}
        {
        }
        Directory(Directory&&) = default;
        virtual ~Directory() = default;
        virtual void accept(Visitor& v) {
            v.visit(*this);
        }
    };

    std::stack<Directory*> dirstack;
    Directory root;

public:
    Filesystem();
    Filesystem(Filesystem&& other) = default;

    Directory& cd(const std::string& name);
    void addFile(const std::string& name, size_t size);
    void addDir(const std::string& name);
    void accept(Visitor& v);
};

#endif
