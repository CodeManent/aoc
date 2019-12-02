#ifndef TEST_CASE_H
#define TEST_CASE_H

#include "Memory.h"

#include <iostream>
#include <algorithm>

class TestCase {
	const int id;
	const Memory initialState;
	const Memory finalState;
public:
	TestCase(const int id, const Memory &m1, const Memory &m2)
		:id(id)
		, initialState(m1)
		, finalState(m2)
	{
	}

	bool Run() const {
		Memory workingState(initialState);

		if (!workingState.Execute()) {
			std::cerr << "Failed computation of test case " << id << std::endl;
			return false;
		}

		if (finalState.size() < workingState.size()) {
			std::cerr << "Final state of test case " << id << " is smaller than the expected result" << std::endl;
			return false;
		}

		for (std::vector<int>::size_type i = 0; i < finalState.size(); ++i) {
			if (finalState[i] != workingState[i]) {
				std::cerr << "Difference at position [" << i << "] for the test case " << id << ": " << finalState[i] << "-" << workingState[i] << std::endl;
				std::cerr << "initial : " << initialState << std::endl;
				std::cerr << "expected: " << finalState << std::endl;
				std::cerr << "actual  : " << workingState << std::endl;
				return false;
			}
		}

		return true;
	}


	static bool RunSystemTests() {

		const std::vector<TestCase> testCases {
			{0, {1,0,0,0,99}, {2,0,0,0,99}},
			{1, {2,3,0,3,99}, {2,3,0,6,99}},
			{2, {2,4,4,5,99,0}, {2,4,4,5,99,9801}},
			{3, {1,1,1,4,99,5,6,0,99}, {30,1,1,4,2,5,6,0,99}},
			{4, {1,0,0,3,99}, {1,0,0,2,99}},
			{5, {1,9,10,3,2,3,11,0,99,30,40,50}, {3500,9,10,70, 2,3,11,0, 99, 30,40,50}}
		};

		return std::all_of(
			std::cbegin(testCases),
			std::cend(testCases),
			[](const TestCase &tc) { return tc.Run(); });
	}

};


#endif
