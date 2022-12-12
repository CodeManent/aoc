#include "FilesystemPrinter.h"

#include "printers.h"

#include <iostream>
#include <ranges>
#include <algorithm>

using namespace std;

FilesystemPrinter::FilesystemPrinter(size_t depth)
	:depth(depth)
{
}

FilesystemPrinter::FilesystemPrinter()
	:FilesystemPrinter(0)
{
}

void FilesystemPrinter::visit(Filesystem::File &f) {
	cout << string(depth * 2, ' ') << "- " << f.name << " (file, size=" << f.size << ")" << endl;
}

void FilesystemPrinter::visit(Filesystem::Directory& d) {
    cout << string(depth * 2, ' ') << "- " << d.name << " (dir)" << endl;
    ++depth;
    ranges::for_each(d.contents, [this](const auto& d) { d->accept(*this); });
    --depth;
}
