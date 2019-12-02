#include "Memory.h"
#include "TestCase.h"

int main(const int argc, const char * const argv[]) {
	if (!TestCase::RunSystemTests()) {
		std::cerr << "Failed on system tests." << std::endl;
		return EXIT_FAILURE;
	}

	auto input = Memory::ReadFromInput();
	if(!input.has_value()){
		std::cerr << "Failed to read the input." << std::endl;
		return EXIT_FAILURE;
	}


	for(int noun = 0; ; ++noun){
		for(int verb = 0; verb <= noun; ++verb) {

			auto memory{input.value()};
			memory[1] = noun;
			memory[2] = verb;

			if (!memory.Execute())
			{
				return EXIT_FAILURE;
			}
			if (memory[0] == 19690720){
				int result = 100 * noun + verb;
				std::cout << "Answer: " << result << std::endl;
				return EXIT_SUCCESS;
			}
		}
	}
}
