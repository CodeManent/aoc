#include "Arcade.h"

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
	std::cout << "Day 13" << std::endl;

	if(!Test()){
		std::cerr << "Tests failed." << std::endl;
		return EXIT_SUCCESS;
	}

	const std::string input{
		std::istreambuf_iterator<char>(std::cin),
		std::istreambuf_iterator<char>()};

	Arcade<long long> arcade(input);
	arcade.Run();
	std::cout << "First " << arcade.CountTiles(Arcade<long long>::Tile::Block) << std::endl;

	Arcade<long long> arcade2(input);
	arcade2.SetFreeToPlay();
	arcade2.Run();
	std::cout << "Second: " << arcade2.GetScore() << std::endl;

	return EXIT_SUCCESS;
}