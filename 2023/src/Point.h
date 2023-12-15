#ifndef Point_H
#define Point_H

#include <iostream>

struct Point{
    long x;
    long y;

    constexpr bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

    constexpr Point operator+(const Point& other) const {
        return Point {
            x + other.x,
            y + other.y
        };
    }

    constexpr Point operator-(const Point& other) const {
        return Point{
            x - other.x,
            y - other.y
        };
    }

    constexpr Point operator*(const long& value) const {
        return Point{
            x * value,
            y * value
        };
    }

    constexpr Point operator/(const long& value) const {
        return Point{
            x / value,
            y / value
        };
    }

    constexpr Point right() const {
        return Point{
            x + 1,
            y
        };
    }

    constexpr Point left() const {
        return Point{
            x - 1,
            y
        };
    }

    constexpr Point up() const {
        return Point{
            x,
            y - 1
        };
    }

    constexpr Point down() const {
        return Point{
            x,
            y + 1
        };
    }
};

std::ostream& operator<<(std::ostream& os, const Point& p) {
    return os << "(" << p.x << ", " << p.y << ")";
}

#endif
