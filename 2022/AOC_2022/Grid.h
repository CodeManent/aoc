#ifndef Grid_h_
#define Grid_h_

#include "SizePoint.h"
#include <functional>

#include <vector>
#include <ostream>

class Grid {
public:
    class Location {
    public:
        typedef short ValueType;

        short treeSize;
        short maxUp;
        short maxDown;
        short maxLeft;
        short maxRight;
        bool isVisible;
        short viewDistanceUp;
        short viewDistanceDown;
        short viewDistanceLeft;
        short viewDistanceRight;
        long scenicScore;

        Location(short treeSize)
            :treeSize(treeSize)
            , maxUp(0)
            , maxDown(0)
            , maxLeft(0)
            , maxRight(0)
            , isVisible(true)
            , viewDistanceUp(0)
            , viewDistanceDown(0)
            , viewDistanceLeft(0)
            , viewDistanceRight(0)
            , scenicScore(0)
        {
        }
    };

    std::vector<Location> backingStore;
    size_t n;

    Grid(size_t n,std::function<Location (const SizePoint&)>);

    const Location& at(const SizePoint& location) const;
    Location& at(const SizePoint& location);

    using LocationVisitor = std::function<void(const SizePoint&, Location&)>;
    using ConstLocationVisitor = std::function<void(const SizePoint&, const Location&)>;

    void visitTopDownLeftRight(LocationVisitor callback);
    void visitTopDownLeftRight(ConstLocationVisitor callback) const;
    void visitBottomUpRightLeft(LocationVisitor callback);
    void visitBottomUpRightLeft(ConstLocationVisitor callback) const;

    void computeVisibility();

    friend std::ostream& operator<<(std::ostream& os, const Grid& g);
};


#endif

