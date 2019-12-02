#include <vector>
#include <tuple>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <functional>
#include <iterator>

template <typename T>
std::ostream& operator << (std::ostream& os, const std::vector<T> &v) {
	os << "{";
	if (v.size() != 0) {
		os << v[0];
	}

	for (auto i = std::cbegin(v) + 1; i != std::cend(v); ++i) {
		os << ", " << *i;
	}

	os << "}";

	return os;
}

class Memory {
	std::vector<int> data;

public:
	Memory(const std::initializer_list<int> &&il)
		:data(il)
	{
	}

	Memory(const std::vector<int> && input, size_t extendTo = 0)
		:data(input)
	{
		if (data.size() < extendTo) {
			data.resize(extendTo);
		}
	}

	Memory(const Memory &other)
		:data(other.data)
	{

	}

	int operator[](const size_t position) const {
		return data.at(position);
	}

	int& operator[](const size_t position) {
		return data.at(position);
	}

	std::vector<int>::size_type size() const {
		return data.size();
	}

	friend std::ostream& operator<<(std::ostream &os, const Memory &m);
};

std::ostream& operator<<(std::ostream &os, const Memory &m) {
	os << m.data;

	return os;
}

bool Execute(Memory &memory) {
	for (std::vector<int>::size_type pc = 0; memory[pc] != 99 ; pc += 4) {

		const auto & opcode = memory[pc];


		int (*op)(int v1, int v4);

		switch (opcode) {
		case 1:
			op = [](const int v1, const int v2) -> int {return v1 + v2; };
			break;

		case 2:
			op = [](const int v1, const int v2) -> int { return v1 * v2; };
			break;

		default:
			std::cerr << "Bad Opcode [" << opcode << "] at position " << pc << std::endl;
			return false;
		}

		const int p1 = memory[pc + 1];
		const int p2 = memory[pc + 2];
		const int to = memory[pc + 3];

		const int v1 = memory[p1];
		const int v2 = memory[p2];

		memory[to] = op(v1, v2);
	}

	return true;
}

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

		if (!Execute(workingState)) {
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
};

bool RunTests() {

	std::vector<TestCase> testCases = {
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

int main(const int argc, const char * const argv[]) {
	if (!RunTests()) {
		return EXIT_FAILURE;
	}

	// Read the program from the input
	std::vector<int> data;

	int tmp;
	if (!std::cin){
		return EXIT_FAILURE;
	}
	std::cin >> tmp;
	data.push_back(tmp);

	while(std::cin && !std::cin.eof()){
		// consume the ,
		std::cin.get();

		if (!std::cin){
			return EXIT_FAILURE;
		}
		//read the number
		std::cin >> tmp;
		data.push_back(tmp);
	}

	Memory memory{std::move(data)};

	// Restore state
	memory[1] = 12;
	memory[2] = 2;

	if(!Execute(memory)){
		return EXIT_FAILURE;
	}

	std::cout << "Memory[0] = " << memory[0] << std::endl;

	return EXIT_SUCCESS;
}

