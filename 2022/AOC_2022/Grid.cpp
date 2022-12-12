#include "Grid.h"
#include "Point.h"

#include <cassert>

Grid::Grid(size_t n, std::function<Grid::Location (const Point&)> locationValueProvider)
    :n(n)
    , backingStore{}
{
    backingStore.reserve(n * n);

    for (size_t y = 0; y < n; ++y) {
        for (size_t x = 0; x < n; ++x) {
            backingStore.emplace_back(locationValueProvider({x, y}));
        }
    }
}

const Grid::Location& Grid::at(const Point& location) const {
    assert(location.x >= 0);
    assert(location.y >= 0);
    assert(location.x <= n);
    assert(location.y <= n);

    return backingStore.at(location.y * n + location.x);
}

Grid::Location& Grid::at(const Point& location) {
    assert(location.x >= 0);
    assert(location.y >= 0);
    assert(location.x <= n);
    assert(location.y <= n);

    return backingStore.at(location.y * n + location.x);
}

void Grid::visitTopDownLeftRight(Grid::LocationVisitor callback) {
    for (size_t y = 0; y < n; ++y) {
        for (size_t x = 0; x < n; ++x) {

            const Point p{ x, y };
            callback(p, at(p));
        }
    }
}

void Grid::visitTopDownLeftRight(Grid::ConstLocationVisitor callback) const {
    for (size_t y = 0; y < n; ++y) {
        for (size_t x = 0; x < n; ++x) {

            const Point p{ x, y };
            callback(p, at(p));
        }
    }
}

void Grid::visitBottomUpRightLeft(Grid::LocationVisitor callback) {
    for (size_t shiftedY = n; shiftedY > 0; --shiftedY) {
        for (size_t shiftedX = n; shiftedX > 0; --shiftedX) {
            // Use shifted[X/Y] because we can't use the actual x/y on the for
            const size_t x = shiftedX - 1;
            const size_t y = shiftedY - 1;

            const Point p{ x, y };

            callback(p, at(p));
        }
    }
}

void Grid::visitBottomUpRightLeft(Grid::ConstLocationVisitor callback) const {
    for (size_t shiftedY = n; shiftedY > 0; --shiftedY) {
        for (size_t shiftedX = n; shiftedX > 0; --shiftedX) {
            // Use shifted[X/Y] because we can't use the actual x/y on the for
            const size_t x = shiftedX - 1;
            const size_t y = shiftedY - 1;

            const Point p{ x, y };
            callback(p, at(p));
        }
    }
}


void Grid::computeVisibility() {
    using std::max;

    // top->bottom, left->right

    visitTopDownLeftRight([this](const Point& loc, Location& current) {
        if (loc.x > 0) {

            const auto& left = at({ loc.x - 1, loc.y });
            current.maxLeft = max(left.maxLeft, left.treeSize);

            current.viewDistanceLeft = 1;
        loop_compute_view_distance_left:
            size_t viewX = loc.x - current.viewDistanceLeft;
            if (viewX > 0) {
                const auto& viewTarget = at({ viewX, loc.y });
                if (current.treeSize > viewTarget.treeSize && viewTarget.viewDistanceLeft > 0) {
                    current.viewDistanceLeft += viewTarget.viewDistanceLeft;
                    goto loop_compute_view_distance_left;
                }
            }
        }

    if (loc.y > 0) {
        const auto& up = at({ loc.x, loc.y - 1 });
        current.maxUp = max(up.maxUp, up.treeSize);

        current.viewDistanceUp = 1;
    loop_compute_view_distance_up:
        size_t viewY = loc.y - current.viewDistanceUp;
        if (viewY > 0) {
            const auto& viewTarget = at({ loc.x, viewY });
            if (current.treeSize > viewTarget.treeSize && viewTarget.viewDistanceUp > 0) {
                current.viewDistanceUp += viewTarget.viewDistanceUp;
                goto loop_compute_view_distance_up;
            }
        }
    }
        });

    visitBottomUpRightLeft([this](const Point& loc, Location& current) {
        if (loc.x + 1 != n) {
            const auto& right = at({ loc.x + 1, loc.y });
            current.maxRight = max(right.maxRight, right.treeSize);

            current.viewDistanceRight = 1;
        loop_compute_view_distance_right:
            size_t viewX = loc.x + current.viewDistanceRight;
            if (viewX < n - 1) {
                const auto& viewTarget = at({ viewX, loc.y });
                if (current.treeSize > viewTarget.treeSize && viewTarget.viewDistanceRight > 0) {
                    current.viewDistanceRight += viewTarget.viewDistanceRight;
                    goto loop_compute_view_distance_right;
                }
            }
        }

    if (loc.y + 1 != n) {
        const auto& down = at({ loc.x, loc.y + 1 });
        current.maxDown = max(down.maxDown, down.treeSize);

        current.viewDistanceDown = 1;
    loop_compute_view_distance_down:
        size_t viewY = loc.y + current.viewDistanceDown;
        if (viewY < n - 1) {
            const auto& viewTarget = at({ loc.x, viewY });
            if (current.treeSize > viewTarget.treeSize && viewTarget.viewDistanceDown > 0) {
                current.viewDistanceDown += viewTarget.viewDistanceDown;
                goto loop_compute_view_distance_down;
            }
        }
    }
        });

    visitTopDownLeftRight([this](const Point& loc, Location& current) {
        current.isVisible = loc.x == 0
        || loc.x == n - 1
        || loc.y == 0
        || loc.y == n - 1
        || current.maxDown < current.treeSize
        || current.maxUp < current.treeSize
        || current.maxLeft < current.treeSize
        || current.maxRight < current.treeSize;

    current.scenicScore = current.viewDistanceLeft
        * current.viewDistanceRight
        * current.viewDistanceUp
        * current.viewDistanceDown;
        });
}