#include <iostream>
#include <cstdlib>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <map>
#include <cstring>
#include <cmath>

#include "Point.cpp"

struct MapNode {
	Point pos;
	int visibleAsteroids;
};

std::ostream &operator<<(std::ostream &os, const Point &p)
{
	os << "(" << p.x << ", " << p.y << ")";
	return os;
}

std::ostream &operator<<(std::ostream &os, const MapNode &n)
{
	os << "Node {" << n.pos << ", " << n.visibleAsteroids << "}";
	return os;
}

constexpr const auto HasAsteroid(const char *const map, const int width, const Point &p)
{
	return map[(p.y * (width + 1)) + p.x] == '#';
}

constexpr const auto CalculateAngle(const Point& p1, const Point& p2){
	const auto d = p2 - p1;

	auto angle = std::atan2(d.y, d.x);
	angle += M_PI/2;

	if(angle >= M_PI){
		angle -= 2*M_PI;
	}

	return angle;
}

std::vector<MapNode> ProcessMap(const char* const map){
	const int width = std::distance(map, std::find(map, map + strlen(map)-1, '\n'));
	const int height = ((strlen(map)-1) / (width));

	std::vector<MapNode> result;


	for (Point start; start.y < height; ++start.y)
	{
		for (start.x = 0; start.x < width; ++start.x)
		{
			if(!HasAsteroid(map, width, start)) {
				continue;
			}
			int asteroidCount = 0;

			for (Point end; end.y < height; ++end.y)
			{
				for (end.x = 0; end.x < width; ++end.x)
				{
					// std::cout << "End " << end << std::endl;

					if(start == end or !HasAsteroid(map, width, end)){
						continue;
					}
					// Check if blocking line of sight
					auto d = end - start;
					if(d.x == 0) {
						d.y = (d.y > 0  ? 1 : -1);
					}
					else if (d.y == 0) {
						d.x = (d.x > 0 ? 1 : -1);
					}
					else{
						int steps = std::gcd(d.x, d.y);
						d.x /= steps;
						d.y /= steps;
					}

					bool blocked = false;
					for(Point testPoint = start + d; !blocked and testPoint != end; testPoint += d){
						blocked = HasAsteroid(map, width, testPoint);
						if(blocked){
							break;
						}
					}

					if(!blocked){
						++asteroidCount;
					}
				}
			}
			result.push_back(MapNode{start, asteroidCount});
		}
	}

	return result;
}

auto Eliminate(const char * const map, const Point &center){
	std::vector<Point> result;
	const int width = std::distance(map, std::find(map, map + strlen(map) - 1, '\n'));
	const int height = ((strlen(map) - 1) / (width));

	bool isEmpty = false;
	while(!isEmpty){
		int remaining = 0;
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				Point current{x, y};
				if(current != center and HasAsteroid(map, width, current) and std::find(result.cbegin(), result.cend(), current) == result.cend()){
					++remaining;
				}
			}
		}
		// std::cout << "Remaining " << remaining << std::endl;
		isEmpty = remaining == 0;

		if (isEmpty) {
			continue;
		}

		// GetVisible
		std::vector<Point> visible;
		for(int y = 0; y < height; ++y){
			for(int x = 0; x < width; ++x){
				Point current{x, y};
				if (current == center or !HasAsteroid(map, width, current) or std::find(result.cbegin(), result.cend(), current) != result.cend()){
					continue;
				}
				// Check if blocking line of sight
				auto d = current - center;
				if (d.x == 0)
				{
					d.y = (d.y > 0 ? 1 : -1);
				}
				else if (d.y == 0)
				{
					d.x = (d.x > 0 ? 1 : -1);
				}
				else
				{
					int steps = std::gcd(d.x, d.y);
					d.x /= steps;
					d.y /= steps;
				}

				bool blocked = false;
				for (Point testPoint = center + d; testPoint != current; testPoint += d)
				{
					if(std::find(result.cbegin(), result.cend(), testPoint) != result.cend()){
						continue;
					}
					blocked = HasAsteroid(map, width, testPoint);
					if(blocked){
						break;
					}
				}

				if (!blocked)
				{
					visible.push_back(current);
				}
			}
		}

		// sort visible by angle
		std::sort(
			visible.begin(),
			visible.end(),
			[&center](const auto &lhs, const auto &rhs){
				const auto angle1 = CalculateAngle(lhs, center);
				const auto angle2 = CalculateAngle(rhs, center);

				return angle1 < angle2;
			});
		
		// add current visible to the result
		std::copy(visible.begin(), visible.end(), std::back_inserter(result));
	}
	return result;
}

const auto BestPosition(const char * const map){
	const auto processed = ProcessMap(map);
	const auto result = std::max_element(processed.begin(), processed.end(), [](const auto &lhs, const auto &rhs) {
		return lhs.visibleAsteroids < rhs.visibleAsteroids;
	});
	return *result;
}

bool Test() {
	const auto input1 = R"(.#..#
.....
#####
....#
...##)";

	auto result1 = BestPosition(input1);
	if(result1.visibleAsteroids != 8){
		std::cout << "Failed test 1 " << std::endl;
		return false;
	}

	const auto input2 = R"(.#..##.###...#######
##.############..##.
.#.######.########.#
.###.#######.####.#.
#####.##.#.##.###.##
..#####..#.#########
####################
#.####....###.#.#.##
##.#################
#####.##.###..####..
..######..##.#######
####.##.####...##..#
.#####..#.######.###
##...#.##########...
#.##########.#######
.####.#.###.###.#.##
....##.##.###..#####
.#.#.###########.###
#.#.#.#####.####.###
###.##.####.##.#..##)";
	auto best2 = BestPosition(input2);
	const auto eliminated = Eliminate(input2, best2.pos);
	struct testCase{
		int i;
		Point p;
	};
	std::vector<testCase> tests = {
		{1, {11, 12}},
		{2, {12, 1}},
		{3, {12, 2}},
		{10, {12, 8}},
		{20, {16, 0}},
		{50, {16, 9}},
		{100, {10, 16}},
		{199, {9, 6}},
		{200, {8, 2}},
		{201, {10, 9}},
		{299, {11, 1}}
	};

	for(const auto t: tests){
		if(eliminated[t.i-1] != t.p){
			std::cout << "Test2 bad case at " << t.i << " expected " << t.p << " actual " << eliminated[t.i-1] << std::endl;
			return false;
		}
	}


	return true;
}

int main(int, char**){
	std::cout << "Day 10" << std::endl;
	if (!Test()){
		std::cerr << "Failed tests." << std::endl;
		return EXIT_FAILURE;
	}
	std::string input{std::istreambuf_iterator<char>{std::cin}, std::istreambuf_iterator<char>{}};
	auto first = BestPosition(input.c_str());
	std::cout << "First " << first.visibleAsteroids << std::endl;

	const auto eliminated = Eliminate(input.c_str(), first.pos);
	if(eliminated.size() < 200){
		std::cerr << "Not enough eliminations." << std::endl;
		return EXIT_FAILURE;
	}

	const auto eliminated200 = eliminated.at(200-1);
	const auto result2 = eliminated200.x * 100 + eliminated200.y;
	std::cout << "Second " << result2 << std::endl;

	return EXIT_SUCCESS;
}
