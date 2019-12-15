#ifndef Arcade_H
#define Arcade_H

#include "IntCodeComputer.h"
#include "Point.h"

#include <stdexcept>
#include <string>
#include <map>
#include <algorithm>
#include <sstream>
#include <thread>

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

template <typename T>
class Arcade{
public:
	enum Tile
	{
		Empty = 0,
		Wall,
		Block,
		Paddle,
		Ball
	};

private:

	struct PointComparator{
		constexpr bool operator()(const Point<T> &lhs, const Point<T> &rhs) const {
			return lhs.y == rhs.y ? lhs.x < rhs.x : lhs.y < rhs.y;
		}
	};
	std::map<Point<T>, Tile, PointComparator> screen;
	IntCodeComputer<T> computer;
	T segmentDisplay = 0;
	T ballX = 0;
	T paddleX = 0;

public:

	Arcade(const std::string &program)
	:computer(program)
	{
	}

	void Draw(const T x, const T y, const Tile tile){
		screen[{x, y}] = tile;
	}

	void Run() {
		int outputState = 0;
		T x;
		T y;
		computer.SetOutputHandler([&](const auto &value) {
			switch(outputState)
			{
			case 0:
				x = value;
				break;
			case 1:
				y = value;
				break;
			case 2:
				if (x == -1 and y == 0){
					segmentDisplay = value;
				}
				else{
					const auto tile = static_cast<Tile>(value);
					switch(tile){
					case Tile::Ball:
						ballX = x;
						break;

					case Tile::Paddle:
						paddleX = x;
						break;	

					default:
						break;
					}

					Draw(x, y, tile);
				}
				break;
			}
			++outputState;
			outputState = outputState % 3;
		});
		computer.SetInputHandler([&](){
			// Render();
			return paddleX > ballX ? -1 : (paddleX < ballX ? 1 : 0);
		});

		computer.Execute();
	}

	auto CountTiles(Tile tile) const {
		return std::count_if(screen.cbegin(), screen.cend(), [&](const auto element){
			return element.second == tile;
		});
	}

	void SetFreeToPlay(){
		computer[0] = 2;
	}

	T GetScore() const {
		return segmentDisplay;
	}

	void Render() const{
		Point<T> previous = screen.cbegin()->first;
		for(const auto &pixel: screen){
			auto diff = pixel.first - previous;
			if (diff.y < 0 ) {
				throw std::runtime_error("Bad pixel y " + std::to_string(pixel.first) + " from " + std::to_string(previous));
			}
			while (diff.y --> 0){
				std::cout << std::endl;
				diff.x = pixel.first.x;
			}
			while (diff.x --> 0){
				std::cout << ' ';
			}
			// Render pixel
			switch(pixel.second){
			case Tile::Empty:
				std::cout << " ";
				break;

			case Tile::Wall:
				std::cout << "W";
				break;

			case Tile::Block:
				std::cout << "B";
				break;

			case Tile::Paddle:
				std::cout << "-";
				break;

			case Tile::Ball:
				std::cout << "o";
				break;

			default:
				throw std::runtime_error("Bad tile." + std::to_string(pixel.second));
			}
			previous = pixel.first;
		}

		std::cout << std::endl;
		std::cout << "Score: " << segmentDisplay << std::endl;
		std::cout.flush();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

};

#endif
