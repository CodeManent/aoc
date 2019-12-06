#include "IntCodeComputer.h"

#include <sstream>

int main(const int argc, const char *const argv[]) {
	IntCodeComputer computer(std::cin);

	computer.SetInputHandler([](){
		std::cout << "Inputing 1" << std::endl;

		return 1;
	});
	computer.SetOutputHandler([](const int value){
		std::cout << "> " << value << std::endl;
	});

	if (!computer.Execute())
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
