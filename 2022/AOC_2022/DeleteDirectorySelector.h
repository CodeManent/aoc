#ifndef DeleteDirectorySelector_h_
#define DeleteDirectorySelector_h_

#include "DirectorySizeComputer.h"

#include <cassert>
#include <string>
#include <optional>

class DeleteDirectorySelector : public DirectorySizeComputer::SizeReporter {
public:
    static const size_t totalSpace = 70000000;
    static const size_t neededSpace = 30000000;

    const size_t usedSpace;
    const size_t needToFree;

    struct SelectedDirectory {
        std::string name;
        std::size_t size;
    };

    std::optional<SelectedDirectory> selected;

    DeleteDirectorySelector(const size_t usedSpace)
        :usedSpace(usedSpace)
        , needToFree(neededSpace - (totalSpace - usedSpace))
        , selected{}
    {
        assert(needToFree > 0);
    }

    virtual void report(Filesystem::Directory& d, size_t size) {
        if (size >= needToFree && (!selected.has_value() || size < selected->size)) {
            selected = std::make_optional<SelectedDirectory>({ d.name, size });
        }
    }
};

#endif
