#ifndef Point_H
#define Point_H

#include <iostream>

template <typename TElem = long>
struct GenericPoint{
    TElem x;
    TElem y;

    constexpr bool operator==(const GenericPoint& other) const {
        return x == other.x && y == other.y;
    }

    constexpr GenericPoint operator+(const GenericPoint& other) const {
        return GenericPoint {
            x + other.x,
            y + other.y
        };
    }

    constexpr GenericPoint operator-(const GenericPoint& other) const {
        return GenericPoint{
            x - other.x,
            y - other.y
        };
    }

    constexpr GenericPoint operator*(const TElem& value) const {
        return GenericPoint{
            x * value,
            y * value
        };
    }

    constexpr GenericPoint operator/(const TElem& value) const {
        return GenericPoint{
            x / value,
            y / value
        };
    }

    constexpr GenericPoint right() const {
        return GenericPoint{
            x + 1,
            y
        };
    }

    constexpr GenericPoint left() const {
        return GenericPoint{
            x - 1,
            y
        };
    }

    constexpr GenericPoint up() const {
        return GenericPoint{
            x,
            y - 1
        };
    }

    constexpr GenericPoint down() const {
        return GenericPoint{
            x,
            y + 1
        };
    }
};

using Point = GenericPoint<long>;

std::ostream& operator<<(std::ostream& os, const Point& p) {
    return os << "(" << p.x << ", " << p.y << ")";
}

#endif
