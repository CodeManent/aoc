#ifndef PointOps_h_
#define PointOps_h_

template <typename T>
class Point2;

template  <typename T>
Point2<T> operator+(const Point2<T>& lhs, const Point2<T>& rhs) {
	return { lhs.x + rhs.x, lhs.y + rhs.y };
}

template  <typename T>
Point2<T> operator-(const Point2<T>& lhs, const Point2<T>& rhs) {
	return { lhs.x - rhs.x, lhs.y - rhs.y };
}

template <typename T>
Point2<T> operator*(const auto& lhs, const Point2<T>& rhs) {
	return { lhs * rhs.x, lhs * rhs.y };
}

template <typename T>
Point2<T> operator*(const Point2<T>& lhs, const auto& rhs) {
	return { lhs.x * rhs, lhs.y * rhs };
}

template <typename T>
Point2<T> operator/(const auto& lhs, const Point2<T>& rhs) {
	return { lhs / rhs.x, lhs / rhs.y };
}

template <typename T>
Point2<T> operator/(const Point2<T>& lhs, const auto& rhs) {
	return { lhs.x / rhs, lhs.y / rhs };
}

template <typename T>
T manhattan(const Point2<T>& lhs, const Point2<T>& rhs) {
	const auto& d = rhs - lhs;
	// inline abs implementation
	return (d.x > T{} ? d.x : -d.x) + (d.y > T{} ? d.y : -d.y); //abs(d.x) + abs(d.y);
}

template <typename T>
auto eucledian(const Point2<T>& lhs, const Point2<T>& rhs) {
	const auto& d = rhs - lhs;
	return sqrt(d.x * d.x + d.y * d.y);
}
#endif
