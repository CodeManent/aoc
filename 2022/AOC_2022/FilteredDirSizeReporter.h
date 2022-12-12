#ifndef FilteredDirSizeReporter_h_
#define FilteredDirSizeReporter_h_

#include "DirectorySizeComputer.h"

class FilteredDirSizeReporter : public DirectorySizeComputer::SizeReporter {
public:
    size_t accumulatedSize = 0;

    virtual void report(Filesystem::Directory& d, size_t size) {
        // cout << "Visited directory: " << d.name << " of size " << accumulatedSize << endl;
        if (size <= 100000) {
            //cout << "Addig " << d.name << " (size: " << size << ")" << endl;
            accumulatedSize += size;
        }
    }
};

#endif // !FilteredDirSizeReporter_h_
