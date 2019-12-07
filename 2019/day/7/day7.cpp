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

struct SelectedConfig
{
	int signal;
	std::vector<int> phases;
};

int ExecuteAmplifierLoop(const IntCodeComputer& reference, const std::vector<int> &phases){
	const auto amplifiercount = phases.size();
	std::vector<IntStream> streams(amplifiercount);
	std::vector<IntCodeComputer> amplifiers(amplifiercount, reference);

	for (auto i = decltype(amplifiercount){0}; i < amplifiercount; ++i) {
		streams[i].Write(phases[i]);
		amplifiers[i].SetInputHandler(std::bind(&IntStream::Read, &streams[i]));
		if (i < phases.size() -1){
			amplifiers[i].SetOutputHandler(std::bind(&IntStream::Write, &streams[i+1], std::placeholders::_1));
		}
	}
	amplifiers.back().SetOutputHandler(std::bind(&IntStream::Write, &streams.front(), std::placeholders::_1));

	streams.front().Write(0);

	// A task-based approach with a thread pool should be better. Good enough for now
	std::vector<std::thread> threads;
	for (auto i = decltype(amplifiercount){0}; i < amplifiercount; ++i)
	{
		threads.emplace_back([&, i](){
			if(!amplifiers[i].Execute()){
				throw std::runtime_error("Amplifier execution failure.");
			}
		});
	}

	std::cerr.flush();

	for(auto &t: threads){
		t.join();
	}

	return streams.front().Read();
}

SelectedConfig FindBestLoopPhases(const IntCodeComputer &templateComputer)
{
	std::vector<int> phases = {5, 6, 7, 8, 9};
	SelectedConfig result = {};

	do
	{
		int curreentSignal = ExecuteAmplifierLoop(templateComputer, phases);
		if (result.signal < curreentSignal)
		{
			result.signal = curreentSignal;
			result.phases = phases;
		}

	} while (std::next_permutation(phases.begin(), phases.end()));

	return result;
}

int ExecuteAmplifierChain(const IntCodeComputer& reference, const std::vector<int> &phases){
	IntStream input;
	IntStream output;

	output.Write(0);

	for(int phase: phases){
		auto amplifier(reference);

		input.Write(phase);
		input.Write(output.Read());


		amplifier.SetInputHandler(std::bind(&IntStream::Read, &input));
		amplifier.SetOutputHandler(std::bind(&IntStream::Write, &output, std::placeholders::_1));
		if(!amplifier.Execute()){
			throw std::runtime_error("Amplifier execution failure.");
		}
	}

	return output.Read();
}

SelectedConfig FindBestPhases(const IntCodeComputer &templateComputer)
{
	std::vector<int> phases = {0, 1, 2, 3, 4};
	SelectedConfig result = {};

	do
	{
		int curreentSignal = ExecuteAmplifierChain(templateComputer, phases);
		if (result.signal < curreentSignal)
		{
			result.signal = curreentSignal;
			result.phases = phases;
		}

	} while (std::next_permutation(phases.begin(), phases.end()));

	return result;
}

bool Test() {
	IntStream is;
	std::vector<int> values = {1, 2, 3, 4};

	for(int v: values){
		is.Write(v);
	}

	for(int v: values){
		int streamValue = is.Read();
		if(streamValue != v) {
			std::cerr << "Bad stream value " << streamValue << ". Was expecting << " << v << "." << std::endl;
			return false;
		}
	}

	struct TestCase{
		int signal;
		std::vector<int> phases;
		std::string program;
	};

	const std::vector<TestCase> tests = {
		{43210, {4, 3, 2, 1, 0}, "3,15,3,16,1002,16,10,16,1,16,15,15,4,15,99,0,0"},
		{54321, {0, 1, 2, 3, 4}, "3,23,3,24,1002,24,10,24,1002,23,-1,23,101,5, 23, 23, 1, 24, 23, 23, 4, 23, 99, 0, 0 "},
		{65210, {1, 0, 4, 3, 2}, "3,31,3,32,1002,32,10,32,1001,31,-2,31,1007,31,0,33,1002,33, 7, 33, 1, 33, 31, 31, 1, 32, 31, 31, 4, 31, 99, 0, 0, 0 "}};
	
	for(const auto &c: tests) {
		std::stringstream ss(c.program);
		const IntCodeComputer testComputer(ss);
		const int result = ExecuteAmplifierChain(testComputer, c.phases);

		if(result != c.signal){
			std::cerr << "Bad result signal " << result << ". Was expecting " << c.signal << "." << std::endl;
			return false;
		}

		const auto computed = FindBestPhases(testComputer);
		if(computed.signal != c.signal){
			std::cerr << "Bad computed signal " << computed.signal << ". Was expecting " << c.signal << "." << std::endl;
			return false;
		}

		if (computed.phases != c.phases){
			std::cerr << "The computed phase configuration "<< computed.phases << " differs from the expected one " << c.phases << std::endl;
			return false;
		}
	}

	const std::vector<TestCase> loopTests = {
		{139629729, {9, 8, 7, 6, 5}, "3,26,1001,26,-4,26,3,27,1002,27,2,27,1,27,26,27,4,27,1001,28,-1,28,1005,28,6,99,0,0,5"},
		{18216, {9, 7, 8, 5, 6}, "3,52,1001,52,-5,52,3,53,1,52,56,54,1007,54,5,55,1005,55,26,1001,54,-5,54,1105,1,12,1,53,54,53,1008,54,0,55,1001,55,1,55,2,53,55,53,4,53,1001,56,-1,56,1005,56,6,99,0,0,0,0,10"}};

	for (const auto c: loopTests) {
		std::stringstream ss(c.program);
		const IntCodeComputer testComputer(ss);
		const int result = ExecuteAmplifierLoop(testComputer, c.phases);

		if (result != c.signal)
		{
			std::cerr << "Bad loop result signal " << result << ". Was expecting " << c.signal << "." << std::endl;
			return false;
		}

		const auto &computed = FindBestLoopPhases(testComputer);
		if (computed.signal != c.signal)
		{
			std::cerr << "Bad loop computed signal " << computed.signal << ". Was expecting " << c.signal << "." << std::endl;
			return false;
		}

		if (computed.phases != c.phases){
			std::cerr << "The computed loop phase configuration "<< computed.phases << " differs from the expected one " << c.phases << std::endl;
			return false;
		}
	}

	return true;
}



int main(const int, const char *const *const) {
	std::cout << "Day 7" << std::endl;

	if(!Test()){
		std::cerr << "Tests failed." << std::endl;
		// return EXIT_SUCCESS;
	}

	const IntCodeComputer computer(std::cin);

	const auto &result = FindBestPhases(computer);
	std::cout << "Highest chain signal: " << result.signal << std::endl;

	const auto &loopResult = FindBestLoopPhases(computer);
	std::cout << "Highest loop signal: " << loopResult.signal << std::endl;
	
	return EXIT_SUCCESS;
}
