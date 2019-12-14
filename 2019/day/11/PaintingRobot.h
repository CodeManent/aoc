#ifndef Painting_Robot_H
#define Painting_Robot_H

#include "IntCodeComputer.h"
#include "Point.h"

#include <functional>
#include <utility>
#include <algorithm>
#include <thread>
#include <map>
#include <vector>

std::ostream &operator << (std::ostream &os, const Point& p){
	os << "Point{" << p.x << ", " << p.y << "}";
	return os;
}

struct PointComparer{
	constexpr const auto operator()(const Point& p1, const Point &p2) const{
		return p1.x == p2.x ? (p1.y < p2.y) : p1.x < p2.x;
	}
};

class PaintingRobot {
public:

	enum Direction {
		Left,
		Right,
		Up,
		Down
	};

	enum PanelColor {
		Black = 0,
		White = 1
	};

	IntCodeComputer<long long> computer;
	Point position;
	Direction direction;

	std::map<Point, PanelColor, PointComparer> region;

	PaintingRobot(const std::string &input)
		: computer{input}
		, position{0, 0}
		, direction{Direction::Up}
	{
	}

	constexpr const auto GetDisplacement(const Direction &d) const {
		switch (d) {
		case Direction::Up:		return Point{0, 1}; 
		case Direction::Down:   return Point{0, -1};
		case Direction::Left:	return Point{-1, 0};
		case Direction::Right:	return Point{1, 0};
		default:
			throw std::runtime_error{"Bad forward direction " + std::to_string(d)};
		}
	}

	void Forward() {
		position += GetDisplacement(direction);
	}

	void Turn(const Direction &d) {
		switch (d) {
		case Direction::Left:
			switch (direction) {
			case Direction::Up: direction = Direction::Left; break;
			case Direction::Down: direction = Direction::Right; break;
			case Direction::Left: direction = Direction::Down; break;
			case Direction::Right: direction = Direction::Up; break;
			default:
				throw std::runtime_error{"Bad forward direction " + std::to_string(direction)};
			}
			break;

		case Direction::Right:
			switch (direction) {
			case Direction::Up: direction = Direction::Right; break;
			case Direction::Down: direction = Direction::Left; break;
			case Direction::Left: direction = Direction::Up; break;
			case Direction::Right: direction = Direction::Down; break;
			default:
				throw std::runtime_error{"Bad forward direction " + std::to_string(direction)};
			}
			break;

		default:
			throw std::runtime_error{"Bad turning direction " + std::to_string(d)};
		}
	}

	void Paint(const PanelColor &c) {
		region[position] = c;
	}

	const auto Sample() {
		auto result = PanelColor::Black;
		if(region.find(position) != region.end()){
			result = region[position];
		}

		return (result == PanelColor::White);
	}

	void Run() {
		computer.SetInputHandler([&, this]() -> int {
			return Sample();
		});

		computer.SetOutputHandler([&, this](const auto &input) {
			static bool toPaint = true;
			if (toPaint) {
				Paint(input ? PanelColor::White : PanelColor::Black);
			}
			else {
				Turn(input ? Direction::Right : Direction::Left);
				Forward();
				// Render();
			}
			toPaint = !toPaint;
		});

		computer.Execute();
	}

	auto CountTiles() const {
		return region.size();
	}

	void Render() const {
		int top = 0;
		int bottom = 0;
		int left = 0;
		int right = 0;

		auto accountFor = [&](const Point &p) {
			top = std::max(top, p.y);
			bottom = std::min(bottom, p.y);
			left = std::min(left, p.x);
			right = std::max(right, p.x);
		};

		for (const auto &elem : region)
		{
			accountFor(elem.first);
		}
		for (const auto &p : {Point{0, 0},
							  position,
							  position + GetDisplacement(direction)})
		{
			accountFor(p);
		}

		auto width = right - left + 1;
		auto height = top - bottom + 1;
		auto project = [=](const Point &p) {
			auto pos = p;
			pos.x += -left;
			pos.y += -bottom;
			// pos.x = width - pos.x;
			pos.y = height - pos.y;
			return pos;
		};

		std::vector<std::vector<char>> fsb(height+1, std::vector<char>(width+1, ' '));
		for(const auto &elem: region){
			auto pos = project(elem.first);
			auto col = elem.second;

			fsb[pos.y][pos.x] = (col == PanelColor::Black ? ' ' : '#');
		}

		const auto o = project({0, 0});
		fsb[o.y][o.x] = 'O';
		const auto p = project(position);
		fsb[p.y][p.x] = 'X';
		const Point dp = project(position + GetDisplacement(direction));
		switch(direction){
			case Direction::Up:    fsb[dp.y][dp.x] = '^'; break;
			case Direction::Down:  fsb[dp.y][dp.x] = 'v'; break;
			case Direction::Left:  fsb[dp.y][dp.x] = '<'; break;
			case Direction::Right: fsb[dp.y][dp.x] = '>'; break;
		}

		for(const auto &row: fsb){
			for(const auto elem: row){
				std::cout << elem;
			}
			std::cout << std::endl;
		}

		std::cout << std::endl;
		std::cout.flush();
		// std::this_thread::sleep_for(std::chrono::microseconds(5000));
	}
};

#endif
