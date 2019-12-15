#include "Point.h"

#include <iostream>
#include <cstdlib>

#include <string>
#include <iterator>
#include <vector>
#include <regex>
#include <numeric>
#include <algorithm>
#include <optional>

struct Body{
	Point position;
	Point gravity;
	Point velocity;

	constexpr int PotentialEnergy() const{
		return std::abs(position.x) + std::abs(position.y) + std::abs(position.z);
	}

	constexpr int KineticEnergy() const{
		return std::abs(velocity.x) + std::abs(velocity.y) + std::abs(velocity.z);
	}

	constexpr int TotalEnergy() const{
		return PotentialEnergy() * KineticEnergy();
	}
};


constexpr int sign(const int val){
	return (0 < val) - (val < 0);	
}

void Step (std::vector<Body> &bodies){
	// Calculate gravity
	for(auto &body: bodies){
		body.gravity = {};
	}

	for (size_t i = 0; i < bodies.size(); ++i)
	{
		auto &b1 = bodies[i];
		for (auto j = i; j < bodies.size(); ++j)
		{
			auto &b2 = bodies[j];
			auto g = b2.position - b1.position;

			b1.gravity.x += sign(g.x);
			b1.gravity.y += sign(g.y);
			b1.gravity.z += sign(g.z);
			b2.gravity.x -= sign(g.x);
			b2.gravity.y -= sign(g.y);
			b2.gravity.z -= sign(g.z);
		}
	}
	
	// Apply gravity
	for(auto &body: bodies){
		body.velocity += body.gravity;
		body.position += body.velocity;
	}
}

int First(const std::vector<Body> &bodies, int steps){
	auto localBodies{bodies};

	for(int i = 0; i < steps; ++i) {
		Step(localBodies);
	}

	return std::accumulate(localBodies.cbegin(), localBodies.cend(), 0, [](int acc, const auto &b) {
		return acc + b.TotalEnergy();
	});
}

auto SecondBruteforce(const std::vector<Body> &bodies){
	auto localBodies{bodies};
	bool finished = false;
	long long steps = 1;
	while(!finished){
		++steps;
		Step(localBodies);
		finished = true;
		for(size_t i = 1; i < localBodies.size(); ++i){
			if(localBodies[i].position != bodies[i].position){
				finished = false;
				break;
			}
		}
	}

	return steps;
}

auto Second(const std::vector<Body> &bodies){
	auto localBodies{bodies};

	bool finished = false;
	long long steps = 1;
	auto loopSteps = std::make_tuple(0ll, 0ll, 0ll);
	auto foundLoop = std::make_tuple(false, false, false);

	while(!finished){
		Step(localBodies);
		++steps;

		if (!std::get<0>(foundLoop)) {
			bool loops = true;
			for (size_t i = 0; i < bodies.size(); ++i) {
				if(bodies[i].position.x != localBodies[i].position.x){
					loops = false;
					break;
				}
			}
			if(loops){
				std::get<0>(foundLoop) = true;
				std::get<0>(loopSteps) = steps;
			}
		}

		if (!std::get<1>(foundLoop)) {
			bool loops = true;
			for (size_t i = 0; i < bodies.size(); ++i) {
				if (bodies[i].position.y != localBodies[i].position.y) {
					loops = false;
					break;
				}
			}
			if(loops){
				std::get<1>(foundLoop) = true;
				std::get<1>(loopSteps) = steps;
			}
		}

		if (!std::get<2>(foundLoop)) {
			bool  loops = true;
			for (size_t i = 0; i < bodies.size(); ++i) {
				if (bodies[i].position.z != localBodies[i].position.z) {
					loops = false;
					break;
				}
			}
			if(loops){
				std::get<2>(foundLoop) = true;
				std::get<2>(loopSteps) = steps;
			}
		}
		finished = std::get<0>(foundLoop) and std::get<1>(foundLoop) and std::get<2>(foundLoop);
	}
	const auto lx = std::get<0>(loopSteps);
	const auto ly = std::get<1>(loopSteps);
	const auto lz = std::get<2>(loopSteps);

	// std::cout << "lx: " << lx << " ly: " << ly << " lz: " << lz << std::endl;

	return std::lcm(lx, std::lcm(ly, lz));
}

std::optional<Body> ParseLine(const std::string &line){
	// <x=-8, y=-10, z=0>
	std::regex bodyPattern{"^<x=(-?[0-9]+), y=(-?[0-9]+), z=(-?[0-9]+)>$"};
	std::smatch m;
	if (!std::regex_match(line, m, bodyPattern))
	{
		return std::nullopt;
	}
	if(m.size() != 4){
		return std::nullopt;
	}
	Point pos{
		atoi(m[1].str().c_str()),
		atoi(m[2].str().c_str()),
		atoi(m[3].str().c_str())
	};

	return std::make_optional(Body{pos, {}, {}});
}

std::vector<Body> ParseInput(std::istream &is){
	std::vector<Body> result;

	while(is){
		std::string line;
		std::getline(is, line);
		const auto parsed = ParseLine(line);
		if(parsed.has_value()){
			result.push_back(parsed.value());
		}
	}

	return result;
}

bool Test(){
	const auto testInput21 = R"(
<x=-1, y=0, z=2>
<x=2, y=-10, z=-7>
<x=4, y=-8, z=8>
<x=3, y=5, z=-1>
)";
	std::istringstream iss21(testInput21);
	const auto bodies21 = ParseInput(iss21);
	const auto secondBrute = SecondBruteforce(bodies21);
	const auto secondResult1 = Second(bodies21);
	if(secondBrute != 2772){
		std::cerr << "Second test1 brute force failed. Expected: 2772 returned " << secondBrute << std::endl;
		return false;
	}
	if (secondResult1 != 2772)
	{
		std::cerr << "Second test1 failed. Expected : 2772 Returned: " << secondResult1 << std::endl;
		return false;
	}

	const auto testInput22 = R"(
<x=-8, y=-10, z=0>
<x=5, y=5, z=10>
<x=2, y=-7, z=3>
<x=9, y=-8, z=-3>)";
	std::istringstream iss22(testInput22);
	const auto secondResult2 = Second(ParseInput(iss22));
	if (secondResult2 != 4686774924l){
		std::cerr << "Second test2 failed. Expected : 4686774924 Returned: " << secondResult2 << std::endl;
		return false;
	}

	return true;
}

int main(int, const char **)
{
	if(!Test()){
		std::cerr << "Tests failed." << std::endl;
		return EXIT_SUCCESS;
	}
	std::cout << "Day 12" << std::endl;

	const auto bodies = ParseInput(std::cin);

	std::cout << "First " << First(bodies, 1000) << std::endl;
	const auto secondResult = Second(bodies) ;
	if (secondResult >= 6732500091353405){
		std::cout << "too big" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "Second " << secondResult << std::endl;

	return EXIT_SUCCESS;
}