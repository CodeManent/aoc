#include "Droid.h"

#include <iostream>
#include <cstdlib>

#include <string>
#include <iterator>
#include <vector>
#include <regex>
#include <numeric>
#include <algorithm>
#include <optional>

#include <map>



bool Test(){

	return true;
}

int main(int, const char **)
{
	std::cout << "Day 15" << std::endl;

	if(!Test()){
		std::cerr << "Tests failed." << std::endl;
		return EXIT_SUCCESS;
	}

	const std::string input{
		std::istreambuf_iterator<char>(std::cin),
		std::istreambuf_iterator<char>()};

	Droid droid(input);
	droid.Explore();
	const auto path = droid.PathToOxygen();
	const auto fillTime = droid.FloodFillOxygen();
	std::cout << "First " << path << std::endl;		// 298
	std::cout << "Second "<< fillTime << std::endl; // 346
	//droid.RenderMap();

	return EXIT_SUCCESS;
}