#include "TaggedStack.h"

#include "printers.h"

#include "Grid.h"

#include <iostream>
#include <iomanip>

using namespace std;

ostream& operator<< (ostream& os, const TaggedStack& tq) {
    return os << "{[" << tq.tag << "]: " << tq.stack << "}";
}

ostream& operator<<(ostream& os, const Grid& g) {
    g.visitTopDownLeftRight([&g](const SizePoint& loc, const Grid::Location& current) {
        cout << setw(2) << current.treeSize;

        if (loc.x + 1 == g.n) {
            cout << endl;
        }
    });

    cout << endl;

    g.visitTopDownLeftRight([&g](const SizePoint& loc, const Grid::Location& current) {
        cout << ' ' << (current.isVisible ? "." : " ");

        if (loc.x + 1 == g.n) {
            cout << endl;
        }
    });
    cout << endl;

    cout << "ViewDistanceLeft" << endl;
    g.visitTopDownLeftRight([&g](const SizePoint& loc, const Grid::Location& current) {
        cout << setw(3) << current.viewDistanceLeft;

        if (loc.x + 1 == g.n) {
            cout << endl;
        }
    });

    cout << "ViewDistanceRight" << endl;
    g.visitTopDownLeftRight([&g](const SizePoint& loc, const Grid::Location& current) {
        cout << setw(3) << current.viewDistanceRight;

        if (loc.x + 1 == g.n) {
            cout << endl;
        }
    });

    cout << "ViewDistanceUp" << endl;
    g.visitTopDownLeftRight([&g](const SizePoint& loc, const Grid::Location& current) {
        cout << setw(3) << current.viewDistanceUp;

        if (loc.x + 1 == g.n) {
            cout << endl;
        }
    });

    cout << "viewDistanceDown" << endl;
    g.visitTopDownLeftRight([&g](const SizePoint& loc, const Grid::Location& current) {
        cout << setw(3) << current.viewDistanceDown;

        if (loc.x + 1 == g.n) {
            cout << endl;
        }
    });

    cout << "Scenic score" << endl;
    g.visitTopDownLeftRight([&g](const SizePoint& loc, const Grid::Location& current) {
        cout << setw(3) << current.scenicScore;

        if (loc.x + 1 == g.n) {
            cout << endl;
        }
    });

    return os;
}


#include "Range.h"

ostream& operator<<(ostream& os, const Range& r) {
    os << '[' << r.start << '-' << r.end << ']';

    return os;
}