#include <iostream>
#include <cstdlib>
#include <map>
#include <string>
#include <iterator>
#include <algorithm>
#include <exception>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <optional>

enum class Direction {
	Up,
	Down,
	Left,
	Right
};

enum class Plane {
	Vertical,
	Horizontal
};

Plane GetPlane(const Direction dir) {
	switch (dir) {
	case Direction::Up:
	case Direction::Down:
		return Plane::Vertical;

	case Direction::Left:
	case Direction::Right:
		return Plane::Horizontal;

	default:
		throw std::domain_error("Bad Direction parameter.");
	}
}

struct Instruction {
	Direction direction;
	int distance;
};

struct Point {
	static const Point Origin;

	int x;
	int y;

	auto step(const Direction &d) const {
		switch (d) {
		case Direction::Up: return Point{ x, y + 1 };
		case Direction::Down: return Point{ x, y - 1 };
		case Direction::Left: return Point{ x - 1, y };
		case Direction::Right: return Point{ x + 1, y };
		default:
			throw std::domain_error("Bad Direction parameter.");
		}
	}

	// Manhattan distance
	int Distance(const Point& other) const {
		return abs(other.x - x) + abs(other.y - y);
	}

	const bool operator== (const Point& other) const {
		return x == other.x && y == other.y;
	}

	const bool operator< (const Point& other) const {
		return Distance(Origin) < other.Distance(Origin);
	}

};
constexpr const Point Point::Origin{ 0, 0 };

struct IntersectionPoint: Point{
	int totalDistance;
};

bool between(int from, int to, int target) {
	return  (from - target) * (to - target) <= 0;
}

struct LineSegment: Instruction {
	int previousDistance;
	Point begin;
	Point end;

	bool Hit(const Point &target) const {
		const auto plane = GetPlane(direction);

		return (plane == Plane::Horizontal && between(begin.x, end.x, target.x) && begin.y == target.y) ||
			(plane == Plane::Vertical && begin.x == target.x && between(begin.y, end.y, target.y));
	}

	std::optional<IntersectionPoint> Intersect(const LineSegment &other) const {
		const auto myPlane = GetPlane(direction);
		const auto otherPlane = GetPlane(other.direction);

		if (myPlane == otherPlane) {
			return std::nullopt;
		}

		Point p = (myPlane == Plane::Horizontal) ? Point{ other.begin.x, begin.y } : Point{ begin.x, other.begin.y };

		if (Hit(p) && other.Hit(p)) {
			int dist = previousDistance + begin.Distance(p) + other.previousDistance + other.begin.Distance(p);

			IntersectionPoint ip { p, dist };
			return ip;
		}
		else{
			return std::nullopt;
		}
	}
};

std::string to_string(const Direction &d) {
	switch (d) {
	case Direction::Up : return "Up";
	case Direction::Down: return "Down";
	case Direction::Left: return "Left";
	case Direction::Right: return "Right";
	default:
		throw std::domain_error("Bad Direction parameter.");
	}
}

Direction ParseDirection(const char c) {
	switch (c) {
	case 'U': return Direction::Up;
	case 'D': return Direction::Down;
	case 'L': return Direction::Left;
	case 'R': return Direction::Right;
	default:
		throw std::domain_error("Bad Direction parameter.");
	}
}

std::ostream& operator<<(std::ostream &os, const Instruction &i) {
	std::cout << "{" << to_string(i.direction) << ", " << i.distance << "}";
	return os;
}

std::ostream& operator<<(std::ostream &os, const Point& p) {
	std::cout << "{" << p.x << ", " << p.y << "}";
	return os;
}

std::ostream &operator<<(std::ostream &os, const IntersectionPoint &ip) {
	std::cout << "{" << ip.x << ", " << ip.y << ", dist: " << ip.totalDistance << "}";
	return os;
}

std::ostream& operator << (std::ostream & os, const LineSegment& s) {
	std::cout << "LineSegment{"<< (Instruction&)(s) << ", " << s.previousDistance << ", " << s.begin << ", " << s.end << "}";
	return os;
}

template<typename T>
std::ostream& operator<< (std::ostream & os, const std::vector<T>& v){
	os << '{';
	auto i = v.begin();
	if (i != v.end() ) {
		os << *i;
	}

	while( ++i != v.end()){
		os << ", " << *i;
	}
	os << '}';

	return os;
}

std::vector<Instruction> Tokenise(const std::string &input) {
	std::vector<Instruction> result;

	std::stringstream ss(input);

	char direction;
	int distance;

	ss >> direction >> distance;
	result.emplace_back(Instruction{ ParseDirection(direction), distance });

	while (ss && !ss.eof()) {
		// consume the ','
		ss.get();
		ss >> direction >> distance;
		result.emplace_back(Instruction{ ParseDirection(direction), distance });
	}

	return result;
}


auto Render(const std::vector<Instruction> &instructions) {
	std::vector<LineSegment> result;
	auto current = Point::Origin;
	int distanceSum = 0;

	for (const auto &i : instructions) {
		LineSegment currentSegment{i.direction, i.distance, distanceSum, current};

		// walk the line
		for (int step = 0; step < i.distance; ++step) {
			current = current.step(i.direction);
		}

		currentSegment.end = current;

		result.emplace_back(currentSegment);

		distanceSum += i.distance;
	}

	return result;
}

auto Render(const LineSegment &segment) {
	std::vector<Point> result;
	result.reserve(segment.distance + 1);

	
	auto current = segment.begin;
	do {
		result.push_back(current);
		current = current.step(segment.direction);

	} while(!(current == segment.end));
	result.push_back(current);

	return result;
}

std::string Draw(const std::vector<std::vector<LineSegment>> &segmentVectors, std::vector<IntersectionPoint> &intersections) {
	auto topLeft = Point::Origin;
	auto bottomRight = Point::Origin;

	for(const auto segments: segmentVectors){
		for (const auto &s : segments) {
			for (const auto &p : { s.begin, s.end }) {
				topLeft.x = std::min(topLeft.x, p.x);
				topLeft.y = std::max(topLeft.y, p.y);

				bottomRight.x = std::max(bottomRight.x, p.x);
				bottomRight.y = std::min(bottomRight.y, p.y);
			}
		}
	}

	const int width = (bottomRight.x - topLeft.x) + 1;
	const int height = (topLeft.y - bottomRight.y) + 1;
	std::vector<char> board(width * (height), ' ');

	const auto ResolvePos = [=, &board](const Point &p) {
		Point onBoard{ p.x -topLeft.x, height - (p.y - bottomRight.y) - 1};
		const size_t pos = onBoard.y * width + onBoard.x;
		
		return pos;
	};

	for (const auto &v : segmentVectors){
		for (const auto &s : v)
		{
			for (const auto &p : Render(s))
			{
				const auto plane = GetPlane(s.direction);

				const size_t pos = ResolvePos(p);
				char &at = board.at(pos);

				if (p == s.begin)
				{

					if (s.begin == Point::Origin)
					{
						at = 'O';
					}
					else
					{
						at = '+';
					}
				}
				else if (plane == Plane::Horizontal)
				{
					at = '-';
				}
				else if (plane == Plane::Vertical)
				{
					at = '|';
				}
				else
				{
					at = ';';
				}
			}
		}
	}

	// Draw intersections
	for (const auto &p: intersections){
		board.at(ResolvePos(p)) = 'X';
	}

	board.at(ResolvePos(Point::Origin)) = 'O';

	std::stringstream ss;
	for(int y = 0; y < height; ++y) {
		ss.write(board.data() + (y*width), width);
		ss.put('\n');
	}

	return ss.str();
}

auto processInput(const std::string &input, int verbose = 0) {
	const auto brakePos = std::find(std::begin(input), std::end(input), '\n');
	if (brakePos == std::end(input)) {
		throw std::runtime_error("No line brake was found in the input.");
	}

	const auto instructions1 = Tokenise(std::string{ std::cbegin(input), brakePos });
	const auto instructions2 = Tokenise(std::string{ brakePos+1, std::cend(input)});

	const auto segments1 = Render(instructions1);
	const auto segments2 = Render(instructions2);

	// std::cout << "Segments1: " << segments1 << std::endl;
	// std::cout << "Segments2: " << segments2 << std::endl;

	std::vector<IntersectionPoint> intersections;

	for (const auto &s1 : segments1) {
		for (const auto &s2 : segments2) {
			const auto &p = s1.Intersect(s2);
			if (p && !(p == Point::Origin)) {
				intersections.push_back(p.value());
			}
		}
	}

	if (verbose > 1) {
		std::cout << Draw({segments1, segments2}, intersections);
	}
	if(verbose) std::cout << intersections.size() << " intersections were found." << std::endl;
	if(intersections.size() == 0) {
		return std::make_pair(0, 0);
	}

	const auto selected = *std::min_element(std::cbegin(intersections), std::cend(intersections));

	const auto result = selected.Distance(Point::Origin);

	if (verbose) std::cout<< "Selected intersection: " << selected << " with distance from " << Point::Origin << " = " << result << std::endl;

	const auto selected2 = *std::min_element(
		std::cbegin(intersections),
		std::cend(intersections),
		[](const auto &p1, const auto &p2) {
			return p1.totalDistance < p2.totalDistance;
		});

	const auto result2 = selected2.totalDistance;

	if (verbose)
		std::cout << "Selected intersection: " << selected2 << " with total line distance from " << Point::Origin << " = " << result2 << std::endl;

	return std::make_pair(result, result2);
}

bool test() {
	const LineSegment s1 = {Direction::Right, 5, 0, {-2, 0}, {2, 0}};
	const LineSegment s2 = {Direction::Down, 5, 0, {1, 2}, {1, -2}};

	auto p1 = Render(s1);
	auto p2 = Render(s2);

	const auto p = s1.Intersect(s2);

	if(!p){
		return false;
	}

	bool success = true;
	std::vector<std::tuple<int, int, std::string>> testCases = {
		{6, 30, "R8,U5,L5,D3\nU7,R6,D4,L4"},
		{159, 610, "R75,D30,R83,U83,L12,D49,R71,U7,L72\nU62,R66,U55,R34,D71,R55,D58,R83"},
		{135, 410, "R98,U47,R26,D63,R33,U87,L62,D20,R33,U53,R51\nU98,R91,D20,R16,D67,R40,U7,R15,U6,R7"}
	};

	for(const auto &tc: testCases){

		const auto result = processInput(std::get<2>(tc));
		if (std::get<0>(tc) != result.first) {
			std::cerr << "Bad first result of " << result.first << " (was expecting: " << std::get<0>(tc) << ")." << std::endl;
			return false;
		}
		if(std::get<1>(tc) != result.second){
			std::cerr << "Bad second result of " << result.second << " (was expecting: " << std::get<1>(tc) << ")." << std::endl;
			return false;
		}
	}



	return success;
}

int main(int, const char*[]) {
	if(!test ()) {
		return EXIT_FAILURE;
	}

	std::string input(
		std::istreambuf_iterator<char>{std::cin},
		std::istreambuf_iterator<char>{});

	processInput(input, 1);

	return EXIT_SUCCESS;
}
