#include "Memory.h"
#include "TestCase.h"

int main(const int argc, const char *const argv[]) {
	if (!TestCase::RunSystemTests()) {
		std::cerr << "Failed on system tests." << std::endl;
		return EXIT_FAILURE;
	}

	auto input = Memory::ReadFromInput();
	if (!input.has_value()) {
		std::cerr << "Failed to read the input." << std::endl;
		return EXIT_FAILURE;
	}

	auto memory{input.value()};

	// Restore state
	memory[1] = 12;
	memory[2] = 2;

	if (!memory.Execute()) {
		return EXIT_FAILURE;
	}

	std::cout << "Memory[0] = " << memory[0] << std::endl;

	return EXIT_SUCCESS;
}
