#include <iostream>
#include <vector>
#include <iterator>
#include <cstdlib>
#include <cmath>
#include <numeric>
#include <map>
#include <algorithm>

long ModuleFuel(long mass){
	return (mass / 3) - 2;
}

long TotalFuel(long mass) {
	if(mass <= 0) return 0;

	long fuel = ModuleFuel(mass);
	if (fuel <= 0) return 0;
	long result = fuel + TotalFuel(fuel);

	return result;
}

int main(int argc, char* argv[] ){

	std::map<long, long> tests = {
		{14, 2},
		{1969, 966},
		{100756, 50346}};

	std::vector<long> badResults = {
		3342351 };

	for (const auto &entry : tests) {
		const auto computed_result = TotalFuel(entry.first);
		if (computed_result != entry.second) {
			std::cerr << "Failed for (" << entry.first << ", " << entry.second << ") "
					  << " computed: " << computed_result
					  << std::endl;
			return EXIT_FAILURE;
		}
	}

	long fuelRequirement = std::transform_reduce(
		std::istream_iterator<long>(std::cin),
		std::istream_iterator<long>(),
		0,
		std::plus<long>(),
		TotalFuel);

	if (std::any_of(std::cbegin(badResults), std::cend(badResults), [&fuelRequirement](const auto &value) { return value == fuelRequirement; })) {
		std::cerr << "Bad Result: " << fuelRequirement << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "Fuel Requirement: " << fuelRequirement << std::endl;

	return EXIT_SUCCESS;
}
