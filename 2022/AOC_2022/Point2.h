#ifndef Point2_h_
#define Point2_h_

template <typename T>
class Point2{
public:
	T x;
	T y;

	Point2()
		:Point2({}, {})
	{
	}

	Point2(T x, T y)
		:x(x)
		, y(y)
	{

	}

	bool operator==(const Point2& rhs) const = default;
};

#endif
