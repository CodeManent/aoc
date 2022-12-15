#ifndef _printers_h_
#define _printers_h_

#include <ostream>
#include <vector>
#include <stack>

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T> data) {
    os << "(";
    size_t index = 0;

    for (auto& i : data) {
        if (index++ != 0) {
            os << ", ";
        }
        os << i;
    }
    os << ")";

    return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::ranges::subrange<T>& r) {
    os << '(';
    if (!r.empty()) {
        auto it = r.begin();
        os << *it;
        while (++it != r.end()) {

            os << ", " << *it;
        }
    }
    os << ')';

    return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::stack<T> q) {
    const auto& c = q._Get_container();
    return os << std::ranges::subrange{ c.begin(), c.end() };
}

class Grid;
std::ostream& operator<<(std::ostream& os, const Grid& g);

template <typename T>
class Point2;

template <typename T>
std::ostream& operator<<(std::ostream& os, const Point2<T>& p) {
    return os << "(" << p.x << ", " << p.y << ")";
}

class Range;

std::ostream& operator<<(std::ostream& os, const Range& r);

#endif // !_printers_h_
