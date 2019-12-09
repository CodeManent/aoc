#include "IntCodeComputer.h"
#include "IntStream.h"

#include <string>
#include <sstream>
#include <functional>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <thread>
#include <type_traits>

bool Test() {
	struct TestCase{
		int signal;
		std::vector<int> phases;
		std::string program;
	};

	bool success = false;

	IntCodeComputer<long long> testComputer1 { "104,1125899906842624,99"};
	testComputer1.SetOutputHandler([&](const auto &value) {
		success = (value == 1125899906842624);
	});
	testComputer1.Execute();
	if(!success) {
		std::cerr << "testComputer1 failed." << std::endl;
		return false;
	}

	IntCodeComputer<long long> testComputer2 {"1102,34915192,34915192,7,4,7,99,0"};
	testComputer2.SetOutputHandler([&](const auto &value) {
		const auto strValue = std::to_string(value);
		success = strValue.size() == 16;
	});
	testComputer2.Execute();
	if (!success){
		std::cerr << "testComputer2 failed." << std::endl;
		return false;
	}

	const std::vector<long long> expected = {109, 1, 204, -1, 1001, 100, 1, 100, 1008, 100, 16, 101, 1006, 101, 0, 99};
	std::vector<long long> quineResult;
	IntCodeComputer<long long> quineTestComputer{"109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99"};
	quineTestComputer.SetOutputHandler([&](const auto &value){
		quineResult.emplace_back(value);
	});
	quineTestComputer.Execute();
	success = expected == quineResult;
	if(!success){
		std::cerr << "quineTestComputer failed" << std::endl;
		std::cerr << "Expected: " << expected << std::endl;
		std::cerr << "Returned: " << quineResult << std::endl;
		return false;
	}

	return true;
}

template <typename T>
void RunOnInput(const IntCodeComputer<T> &computer, const T& input)
{
	auto localCopy{computer};
	localCopy.SetInputHandler([&](){
		return input;
	});
	localCopy.SetOutputHandler([&](const auto & value){
		std::cout << " (" << input << ") > " << value << std::endl;
	});
	localCopy.Execute();
}

int main(const int, const char *const *const) {
	std::cout << "Day 9" << std::endl;

	if(!Test()){
		std::cerr << "Tests failed." << std::endl;
		return EXIT_FAILURE;
	}

	const IntCodeComputer<long long> computer(std::cin);
	RunOnInput(computer, 1LL);
	RunOnInput(computer, 2LL);

	return EXIT_SUCCESS;
}
