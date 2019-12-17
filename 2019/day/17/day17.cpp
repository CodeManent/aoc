#include "IntCodeComputer.h"
#include "IntStream.h"
#include "Point.h"
#include "Utilities.h"

#include <iostream>
#include <cstdlib>
#include <functional>
#include <map>


#include <string>
#include <iterator>
#include <vector>
#include <regex>
#include <numeric>
#include <algorithm>
#include <optional>



bool Test(){

	return true;
}

enum Tile {
	Scaffold = '#',
	Empty = '.',
	EndOFLine = '\n',
	RobotUp = '^',
	RobotDown = 'v',
	RobotLeft = '<',
	RobotRight = '>',
	RobotFalling = 'X',
	Intersection = 'O'
};
enum Action
{
	TurnLeft = 'L',
	TurnRight = 'R'
};

int main(int, const char **)
{
	std::cout << "Day 17" << std::endl;

	if(!Test()){
		std::cerr << "Tests failed." << std::endl;
		return EXIT_SUCCESS;
	}

	const std::string input{
		std::istreambuf_iterator<char>(std::cin),
		std::istreambuf_iterator<char>()};

	IntCodeComputer<long long> computer(input);
	IntStream<long long> computerOutput;
	computer.SetOutputHandler(std::bind(&IntStream<long long>::Write, &computerOutput, std::placeholders::_1));
	computer.Execute();

	std::map<Point<long long>, Tile, PointComparator> screen;
	{
		Point<long long> p{0, 1};
		while(!computerOutput.empty()){
			const auto t = static_cast<Tile>(computerOutput.Read());
			++p.x;
			screen[p] = t;
			if (t == Tile::EndOFLine)
			{
				p.y += 1;
				p.x = 0;
			}
		}
	}

	std::vector<Point<long long>> intersections;
	for(const auto &kvp: screen){
		if (screen.find(kvp.first) != screen.end() && screen[kvp.first] == Tile::Scaffold) {
			bool isCrossing = true;
			for (const auto &dir : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}){
				const auto testPoint = kvp.first + PointDirection(dir);
				if (screen.find(testPoint) == screen.end() or screen[testPoint] != Tile::Scaffold){
					isCrossing = false;
				}
			}
			if(isCrossing){
				intersections.emplace_back(kvp.first);
			}
		}
	}

	// std::cout << screen.size() << " points on the map of which " <<  intersections.size() << " intersections" << std::endl;

	const auto alignmentSum = std::accumulate(
		intersections.cbegin(),
		intersections.cend(),
		0,
		[screen](const auto acc, const auto &p) {
			return acc + ((p.x -1) * (p.y-1));
		});
	
	std::cout << "First " << alignmentSum << std::endl;

	// find robot position and direction
	Direction robotDir;
	Point<long long> robotPos;
	for(const auto &kvp: screen){
		robotPos = kvp.first;
		if (kvp.second == Tile::RobotUp) {
			robotDir = Direction::Up;
			break;
		}else if(kvp.second == Tile::RobotDown){
			robotDir = Direction::Down;
			break;
		}else if(kvp.second == Tile::RobotLeft){
			robotDir = Direction::Left;
			break;
		}else if(kvp.second == Tile::RobotRight){
			robotDir = Direction::Right;
			break;
		}
	}

	// Walk the scaffold and record the actions
	std::vector<Action> actions;
	int streak = 0;

	while(true){
		// walk straingh
		auto nextPos = robotPos + PointDirection(robotDir);
		if(screen.find(nextPos) != screen.end() and  screen.at(nextPos) == Tile::Scaffold){
			robotPos = nextPos;
			++streak;
			continue;
		}
		if(streak != 0){
			actions.push_back(static_cast<Action>(streak));
			streak = 0;
		}

		// turn
		Direction nextDir = Turn(robotDir, Rotation::CCW);
		nextPos = robotPos + PointDirection(nextDir);
		if (screen.find(nextPos) != screen.end() and screen.at(nextPos) == Tile::Scaffold){
			actions.push_back(Action::TurnLeft);
			robotDir = nextDir;
			continue;
		}

		nextDir = Turn(robotDir, Rotation::CW);
		nextPos = robotPos + PointDirection(nextDir);
		if (screen.find(nextPos) != screen.end() and screen.at(nextPos) == Tile::Scaffold)
		{
			actions.push_back(Action::TurnRight);
			robotDir = nextDir;
			continue;
		}

		// finished the scaffold
		break;
	}
	std::cout << std::endl;

	std::cout << "Actions : ";
	for(const auto a: actions){
		if (a == Action::TurnLeft){
			std::cout << 'L';
		}
		else if (a == Action::TurnRight)
		{
			std::cout << 'R';
		}
		else{
			std::cout << int(a);
		}
		std::cout << ",";
	}
	std::cout << std::endl;

	// Actions output for my input is L,10,R,8,R,8,L,10,R,8,R,8,L,10,L,12,R,8,R,10,R,10,L,12,R,10,L,10,L,12,R,8,R,10,R,10,L,12,R,10,L,10,L,12,R,8,R,10,R,10,L,12,R,10,R,10,L,12,R,10,L,10,R,8,R,8

	// A: L,10,R,8,R,8
	// B: L,10,L,12,R,8,R,10
	// C: R,10,L,12,R,10

	// Main routine: A,A,B,C,B,C,B,C,C,A

	IntCodeComputer<long long> robot{input};
	robot[0] = 2;

	IntStream<long long> robotInput;
	// IntStream<long long> robotOutput;
	robot.SetInputHandler([&robotInput]() {
		const auto res = robotInput.Read();
		std::cout << (char) res;
		return res;
	});
	// robot.SetOutputHandler(std::bind(decltype(robotOutput)::Write, &robotOutput, std::placeholders::_1));
	const char *inputStr = "A,A,B,C,B,C,B,C,C,A" // InputProgram
						   "\n"					 // split
						   "L,10,R,8,R,8"		 // A
						   "\n"					 // split
						   "L,10,L,12,R,8,R,10"  // B
						   "\n"					 // split
						   "R,10,L,12,R,10"		 // C
						   "\n"					 // split
						   "n"					 // yes video feed
						   "\n";

	for(const char* ch= inputStr; *ch != '\0'; ++ch){
		robotInput.Write(*ch);
	}

	robot.SetOutputHandler([](const auto value){
		if(value < 255){
			std::cout << (char) value;
		}
		else{
			std::cout << "Dust collected: " << value << std::endl;
		}
	});
	robot.Execute();

	return EXIT_SUCCESS;
}