#ifndef FilesystemPrinter_h_
#define FilesystemPrinter_h_

#include "Filesystem.h"

class FilesystemPrinter : public Filesystem::Visitor {
    size_t depth = 0;
public:
    FilesystemPrinter(size_t depth);
    FilesystemPrinter();

    virtual void visit(Filesystem::File& f);
    virtual void visit(Filesystem::Directory& d);
};

#endif
