#ifndef Point_H
#define Point_H

struct Point {
	int x;
	int y;

	constexpr Point()
		: Point{0, 0}
	{
	}

	constexpr Point(const int &x, const int &y)
		: x{ x }, y{ y }
	{
	}

	constexpr Point(const Point &other)
		: x{ other.x }, y{ other.y }
	{
	}

	constexpr Point(const Point &&other)
		: x{other.x}, y{other.y}
	{
	}

	constexpr const auto operator=(const Point &other){
		x = other.x;
		y = other.y;
		
		return *this;
	}

	constexpr auto operator+=(const Point &rhs) {
		this->x += rhs.x;
		this->y += rhs.y;

		return *this;
	}
	constexpr auto operator-=(const Point&& rhs) {
		x -= rhs.x;
		y = rhs.y;

		return *this;
	}

	constexpr auto operator+(const Point &rhs) const {
		return Point{ x + rhs.x, y + rhs.y };
	}

	constexpr auto operator-(const Point &rhs) const {
		return Point{ x - rhs.x, y - rhs.y };
	}

	constexpr auto operator==(const Point &rhs) const {
		return x == rhs.x and y == rhs.y;
	}

	constexpr auto operator!=(const Point &rhs) const {
		return !operator==(rhs);
	}

	constexpr auto operator==(const Point &&rhs) const {
		return x == rhs.x and y == rhs.y;
	}

	constexpr auto operator!=(const Point &&rhs) const {
		return !operator==(rhs);
	}
};

#endif
