#include <iostream>
#include <vector>
#include <iterator>
#include <cstdlib>
#include <cmath>
#include <numeric>

int ModuleFuel(long mass){
	return (mass / 3) - 2;
}

int main(int argc, char* argv[] ){

	long fuelRequirement = std::transform_reduce(
		std::istream_iterator<long>(std::cin),
		std::istream_iterator<long>(),
		0,
		std::plus<long>(),
		ModuleFuel);

	std::cout << "Fuel Requirement: " << fuelRequirement << std::endl;

	return EXIT_SUCCESS;
}
