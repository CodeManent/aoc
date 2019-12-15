#ifndef Point_H
#define Point_H

template <typename T>
struct Point {
	T x;
	T y;
	T z;

	constexpr Point()
		: Point{0, 0, 0}
	{
	}

	constexpr Point(const T &x, const T &y)
		: x{ x }, y{ y }, z{0}
	{
	}

	constexpr Point(const T &x, const T &y, const T &z)
		: x{x}, y{y}, z{z}
	{
	}

	constexpr Point(const Point &other)
		: x{ other.x }, y{ other.y }, z{other.z}
	{
	}

	constexpr Point(const Point &&other)
		: x{other.x}, y{other.y}, z{other.z}
	{
	}

	constexpr const auto operator=(const Point &other){
		x = other.x;
		y = other.y;
		z = other.z;

		return *this;
	}

	constexpr auto operator+=(const Point &rhs) {
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;

		return *this;
	}
	constexpr auto operator-=(const Point&& rhs) {
		x -= rhs.x;
		y = rhs.y;
		z = rhs.z;

		return *this;
	}

	constexpr auto operator+(const Point &rhs) const {
		return Point{ x + rhs.x, y + rhs.y , z + rhs.z};
	}

	constexpr auto operator-(const Point &rhs) const {
		return Point{ x - rhs.x, y - rhs.y, z - rhs.z};
	}

	constexpr auto operator==(const Point &rhs) const {
		return x == rhs.x and y == rhs.y and z == rhs.z;
	}

	constexpr auto operator!=(const Point &rhs) const {
		return !operator==(rhs);
	}

	constexpr auto operator==(const Point &&rhs) const {
		return x == rhs.x and y == rhs.y and z == rhs.z;
	}

	constexpr auto operator!=(const Point &&rhs) const {
		return !operator==(rhs);
	}
};

#endif
