#include "PaintingRobot.h"

#include <iostream>
#include <cstdlib>

#include <string>
#include <iterator>

int main(int, const char**) {

	const std::string program {
		std::istreambuf_iterator<char>(std::cin),
		std::istreambuf_iterator<char>()};

	PaintingRobot robot{program};
	robot.Run();
	robot.Render();
	std::cout << "First " << robot.CountTiles() << std::endl;

	PaintingRobot robbot2{program};
	robbot2.Paint(PaintingRobot::PanelColor::White);
	robbot2.Run();
	robbot2.Render();

	return EXIT_SUCCESS;
}