#ifndef DirectorySizeComputer_h_
#define DirectorySizeComputer_h_

#include "Filesystem.h"

class DirectorySizeComputer : public Filesystem::Visitor {
public:
    class SizeReporter {
    public:
        virtual void report(Filesystem::File&, size_t) {
            //noop by default
        }
        virtual void report(Filesystem::Directory&, size_t) {
            //noop by default
        }
    };

    SizeReporter& reporter;
    size_t dirSize = 0;

    DirectorySizeComputer(SizeReporter& r)
        :reporter(r)
    {
    }

    virtual void visit(Filesystem::File& f) {
        dirSize += f.size;
        reporter.report(f, f.size);
    }

    virtual void visit(Filesystem::Directory& d) {
        for (const auto& entry : d.contents) {
            DirectorySizeComputer subdirVisitor(reporter);

            entry->accept(subdirVisitor);

            dirSize += subdirVisitor.dirSize;
        }

        reporter.report(d, dirSize);
    }
};

#endif // !DirectorySizeComputer_h_
