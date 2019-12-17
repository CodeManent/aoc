#ifndef Utilities_H
#define Utilities_H

#include <ostream>
#include <stdexcept>

#include "Point.h"

template <typename T>
std::ostream &operator<<(std::ostream &os, const Point<T> &p)
{
	os << "Point(" << p.x << ", " << p.y << ")";
	return os;
}

enum Direction {
	Up = 1,
	Down = 2,
	Left = 3,
	Right = 4
};

enum Rotation {
	CW,
	CCW
};

constexpr Point<long long> PointDirection(const Direction &dir)
{
	switch (dir)
	{
	case Direction::Up:
		return {0, -1};
	case Direction::Down:
		return {0, 1};
	case Direction::Left:
		return {-1, 0};
	case Direction::Right:
		return {1, 0};
	default:
		throw std::domain_error("Bad direction.");
	}
}

constexpr Direction Turn(const Direction dir, const Rotation rot){
	switch(rot){
		case Rotation::CW:
		switch(dir){
			case Direction::Up: return Direction::Right;
			case Direction::Right: return Direction::Down;
			case Direction::Down: return Direction::Left;
			case Direction::Left: return Direction::Up;
		}
		break;
		case Rotation::CCW:
		switch (dir){
			case Direction::Up: return Direction::Left;
			case Direction::Left: return Direction::Down;
			case Direction::Down: return Direction::Right;
			case Direction::Right: return Direction::Up;
		}
	}

	throw std::runtime_error("Bad Turn");
}

struct PointComparator
{
	constexpr bool operator()(const Point<long long> &lhs, const Point<long long> &rhs) const
	{
		return lhs.y == rhs.y ? lhs.x < rhs.x : lhs.y < rhs.y;
	}
};

#endif
