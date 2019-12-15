#ifndef Droid_H
#define Droid_H

#include "IntCodeComputer.h"
#include "Point.h"

#include <stdexcept>
#include <string>
#include <map>
#include <algorithm>
#include <sstream>
#include <thread>
#include <stack>
#include <vector>


template <typename T>
std::ostream& operator<<(std::ostream& os, const Point<T> &p){
	os << "Point(" << p.x << ", " << p.y << ")";
	return os;
}

namespace std{
	template <typename T>
	std::string to_string(const Point<T>& p) {
		std::ostringstream oss;
		oss << p;
		return oss.str();
	}
}

class Droid{
public:
	enum Command
	{
		Invalid = 0,
		North = 1,
		South = 2,
		West = 3,
		East = 4
	};

	constexpr Droid::Command GetReverse(const Droid::Command &cmd) const {
		switch (cmd)
		{
			case Command::North: return Command::South;
			case Command::South: return Command::North;
			case Command::West: return Command::East;
			case Command::East: return Command::West;
			default:
				throw std::runtime_error("Invalid command.");
		}
	}

	constexpr const Point<long long> Step(const Droid::Command &cmd) const {
		switch(cmd){
			case Command::North: return {0, 1};
			case Command::South: return {0, -1};
			case Command::West: return {-1, 0};
			case Command::East: return {1, 0};
			default:
				throw std::runtime_error("Invalid command.");
		}
	}

	enum Status{
		HitWall = 0,
		Moved = 1,
		FoundOxygen = 2
	};

private:

	struct PointComparator{
		constexpr bool operator()(const Point<long long> &lhs, const Point<long long> &rhs) const {
			return lhs.y == rhs.y ? lhs.x < rhs.x : lhs.y < rhs.y;
		}
	};
	// Wall : -1. Steps otherwise
	std::map<Point<long long>, int, PointComparator> map;
	IntCodeComputer<long long> computer;
	Point<long long> pos;
	Point<long long> oxygenSystem;

public:
	Droid(const std::string &program)
		: computer(program), pos{}, oxygenSystem{}
	{
	}

	int PathToOxygen() const{
		if (map.find(oxygenSystem) != map.end()){
			return map.at(oxygenSystem);
		};
		return 0;
	}

	auto Explore() {
		std::stack<Point<long long>> path;
		pos = {0, 0};
		map[pos] = 0;
		path.push(pos);

		computer.SetInputHandler([&, this]() {
			// Find the next unexplored
			for (const auto dir : {Command::North, Command::South, Command::West, Command::East}){
				RenderMap();
				auto nextPos = pos + Step(dir);
				if(map[nextPos] == 0) {
					map[nextPos] = map[pos] + 1;
					path.push(nextPos);

					return dir;
				}
			}

			// No next point is unexplored, backtrack
			path.pop();
			if(path.size() == 0){
				computer.Stop();
				return Command::Invalid;
			}
			for (const auto dir : {Command::North, Command::South, Command::West, Command::East}){
				// find how to backtrack
				if((pos + Step(dir)) == path.top()){
					return dir;
				}
			}

			throw std::runtime_error("Couldn't decide for next pos.");
			return Command::Invalid;
		});
		computer.SetOutputHandler([&, this](const auto &value) {
			if(value == Status::HitWall){
				map[path.top()] = -1;
				path.pop();
			}
			else if(value == Status::FoundOxygen){
				oxygenSystem = path.top();
			}
			
			Point<long long> p = path.top();
			pos = p;
		});

		computer.Execute();
	}

	int FloodFillOxygen() {
		int steps = 0;
		std::vector<Point<long long>> filled;
		std::vector<Point<long long>> horizon;

		horizon.push_back(oxygenSystem);
		while(horizon.size()){
			RenderMap();
			// extend the horizon
			std::vector<Point<long long>> newHorizon;
			for(const auto p: horizon){
				for (const auto dir : {Command::North, Command::South, Command::West, Command::East}){
					const auto next = p + Step(dir);
					if (map.at(next) != -1 and
						std::find(newHorizon.begin(), newHorizon.end(), next) == newHorizon.end() and
						std::find(horizon.begin(), horizon.end(), next) == horizon.end() and
						std::find(filled.begin(), filled.end(), next) == filled.end())
					{
						newHorizon.push_back(next);
					}
				}
			}
			if(newHorizon.size()){
				++steps;
			}

			// mark the map
			for(const auto p: horizon){
				map[p] = 1000;
			}
			// copy the current horizon to the visited memory
			std::copy(horizon.begin(), horizon.end(), std::back_inserter(filled));
			for (const auto p : horizon) {
				map[p] = 1000;
			}

			horizon.swap(newHorizon);
			for (const auto p : horizon) {
				map[p] = 1001;
			}
		}

		return steps;
	}

	void RenderMap() const{
		std::cout << "\033[2J\033[1;1H";

		Point<long long> minP = {};
		Point<long long> maxP = {};
		for (const auto &m : map)
		{
			minP.x = std::min(m.first.x, minP.x);
			minP.y = std::min(m.first.y, minP.y);
			maxP.x = std::max(m.first.x, maxP.x);
			maxP.y = std::max(m.first.y, maxP.y);
		}

		auto project = [&](const Point<long long> &p){
			auto result = p;
			result.x += -minP.x;
			result.y += -minP.y;
			return result;
		};

		size_t width = maxP.x - minP.x;
		size_t height = maxP.y - minP.y + 1;
		std::vector<std::vector<char>> fsb(height + 1, std::vector<char>(width + 1, ' '));
		for(const auto &m: map){
			auto p = project(m.first);

			if (m.first == pos){
				fsb[p.y][p.x] = 'X';
			}
			else if (m.first == Point<long long>{0, 0}){
				fsb[p.y][p.x] = 'O';
			}
			else{
				switch (m.second){
				case -1:
					fsb[p.y][p.x] = 'W';
					break;
				case 0:
					fsb[p.y][p.x] = '?';
					break;

				case 1000:
					fsb[p.y][p.x] = '.';
					break;

				case 1001:
					fsb[p.y][p.x] = '0';
					break;

				default:
					fsb[p.y][p.x] = ' ';
					break;
				}
			}
		}
		for (const auto &row : fsb) {
			for (const auto elem : row) {
				std::cout << elem;
			}
			std::cout << '\n';
		}

		std::cout << std::endl;
		std::cout.flush();
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
	}

};

#endif
