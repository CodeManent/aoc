#include <iostream>
#include <cstdlib>
#include <vector>
#include <cctype>
#include <stdexcept>
#include <string>
#include <iterator>

template <typename T>
std::string to_string(const std::vector<T> &input){
	std::vector<char> result;

	for(size_t i = 0; i < input.size(); ++i){
		result.emplace_back('0' + input[i]);
	}
	std::string output{result.cbegin(), result.cend()};

	return output;
}

constexpr auto KeyStream(const size_t i, const size_t j) {
	constexpr const int key[4] = { 0, 1, 0, -1 };
	constexpr const auto keyLength = sizeof(key) / sizeof(key[0]);

	auto index = ((j + 1) / (i + 1)) % keyLength;

	return key[index];
}

auto FFT(const std::vector<long long> &input) {
	std::vector<long long> result(input.size(), 0);

	for (size_t i = 0; i < input.size(); ++i) {
		for (size_t j = 0; j < input.size(); ++j) {
			result[i] += input[j] * KeyStream(i, j);
		}
	}

	for (auto & i : result) {
		i = std::abs(i % 10);
	}
	return result;
}

auto First(const std::vector<long long> &input) {
	if(input.size() < 8){
		throw std::length_error("Input is too small.");
	}

	auto localResult = input;
	for (int i = 0; i < 100; ++i) {
		localResult = FFT(localResult);
	}

	return decltype(localResult){localResult.begin(), localResult.begin() + 8};
}

auto ParseInput(const std::string &input) {
	using namespace std::string_literals;

	std::vector<long long> result;
	result.reserve(input.size());

	for (auto iter = input.cbegin(); iter != input.cend(); ++iter) {
		if (not std::isdigit(*iter)) {
			if(not(std::isblank(*iter) or std::iscntrl(*iter))){
				throw std::runtime_error("Invalid character was found \""s + *iter + "\".");
			}
		}
		else{
			result.emplace_back(*iter - '0');
		}
	}

	return result;
}

// // Too Slow
// auto Second(const std::string input)
// {
// 	// second
// 	auto offset = std::atol(input.substr(0, 7).c_str());

// 	const auto parsedInput = ParseInput(input);
// 	const auto initialSize = parsedInput.size();
// 	std::vector<long long> parsedReal(initialSize * 10000, 0);
// 	std::cout << "Size " << parsedReal.size() << std::endl;
// 	std::cout << "Offset: " << offset << std::endl;

// 	// fill real signal
// 	for (int i = 0; i < 10000; ++i){
// 		std::copy(parsedInput.cbegin(), parsedInput.cend(), &(parsedReal[i*initialSize]));
// 	}

// 	std::vector<long long> tmpVector(parsedReal.size());
// 	for (int iter = 0; iter < 100; ++iter){
// 		std::cout << "Second Computing iteration " << iter << std::endl;

// 		// clear the tmpVector
// 		for (auto &i : tmpVector)
// 			i = 0;

// 		for (long line = offset; line < long(parsedReal.size()); ++line)
// 		{
// 			if(line % 1000 == 0){
// 				std::cout << line << std::endl;
// 			}

// 			long lineSum = 0;
// 			for(long column = parsedReal.size() - 1; column >= line; --column){
// 				const auto key = KeyStream(line, column);
// 				if(key != 0){
// 					lineSum += parsedReal[column] * key;
// 				}
// 			}
// 			tmpVector[line] += lineSum;
// 		}

// 		for (auto &i : tmpVector)
// 		{
// 			i = std::abs(i % 10);
// 		}
// 		parsedReal.swap(tmpVector);
// 	}

// 	std::cout << "Value at offset ";
// 	 for (int i = 0; i < 8; ++i) {
// 		std::cout << parsedReal[offset + i];
// 	}
// 	std::cout << std::endl;

// 	return decltype(parsedReal)(parsedReal.begin()+offset, parsedReal.begin() +offset + 8);
// }

template <typename T>
std::ostream& operator<<(std::ostream & os, const std::vector<T> &v) {
	os << '(';
	auto iter = v.cbegin();
	if (iter != v.cend()) {
		os << *iter;
		while (++iter != v.end()) {
			os << ", " << *iter;
		}
	}
	os << ')';
	return os;
}

bool Test() {
	const std::vector<std::vector<long long>> keys = {
		{1, 0, -1, 0, 1,  0, -1, 0},
		{0, 1,  1, 0, 0, -1, -1, 0},
		{0, 0,  1, 1, 1,  0,  0, 0},
		{0, 0,  0, 1, 1,  1,  1, 0},
		{0, 0,  0, 0, 1,  1,  1, 1},
		{0, 0,  0, 0, 0,  1,  1, 1},
		{0, 0,  0, 0, 0,  0,  1, 1},
		{0, 0,  0, 0, 0,  0,  0, 1}
	};

	for (size_t i = 0; i < keys.size(); ++i) {
		for (size_t j = 0; j < keys[i].size(); ++j) {
			const auto computedKey = KeyStream(i, j);
			if (keys[i][j] != computedKey) {
				std::cerr
					<< "Failed to validate the key stream on i = " << i
					<< ", j = " << j
					<< ". Was expecting " << keys[i][j]
					<< " but received " << computedKey << std::endl;
				return false;
			}
		}
	}

	auto v1 = ParseInput("12345678");
	if(v1 != decltype(v1){1, 2, 3, 4, 5, 6, 7, 8}) {
		std::cerr << "Failed to parse \"12345678\"." << std::endl;
		return false;
	}

	const char * const phaseResults[] = {
		"48226158",
		"34040438",
		"03415518",
		"01029498"
	};

	for (size_t i = 0; i < 4; ++i) {
		v1 = FFT(v1);
		if (ParseInput(phaseResults[i]) != v1) {
			std::cerr << "Bad phase " << i+1 << " result: " << v1 << std::endl;
			std::cerr << "Was expecting:      " << ParseInput(phaseResults[i]) << std::endl;
			return false;
		}
	}

	struct FFTTestCase {
		const char * const input;
		const char * const expectedResult;
	} TestCases[] = {
		{"80871224585914546619083218645595", "24176176"},
		{"19617804207202209144916044189917", "73745418"},
		{"69317163492948606335995924319873", "52432133"}
	};
	for (const auto tcase: TestCases) {
		auto testResult = to_string(First(ParseInput(tcase.input)));
		// const auto expected = ParseInput(tcase.expectedResult);

		if (testResult != tcase.expectedResult) {
			std::cout << "Failed to verify the result of test case \"" << tcase.input
					  << "\" against the expected result \"" << tcase.expectedResult
					  << ". The computed value is " << testResult << std::endl;
			return false;
		}
	}

	// FFTTestCase TestCases2[] = {
	// 	{"03036732577212944063491565474664", "84462026"},
	// 	{"02935109699940807407585447034323", "78725270"},
	// 	{"03081770884921959731165446850517", "53553731"}
	// };
	// for(const auto &tcase: TestCases2){
	// 	auto testResult = to_string(Second(tcase.input));

	// 	if(testResult != tcase.expectedResult){
	// 		std::cout << "Failed to verify the result of test 2 case \"" << tcase.input
	// 				  << "\" against the expected result \"" << tcase.expectedResult
	// 				  << ". The computed value is " << testResult << std::endl;
	// 		return false;
	// 	}

	// }

	return true;
}

int main(int, const char* const * const) {
	std::cout << "Day 16" << std::endl;
	if (!Test()) {
		std::cerr << "Tests Failed." << std::endl;
		return EXIT_FAILURE;
	}

	const std::string input{
		std::istreambuf_iterator<char>(std::cin),
		std::istreambuf_iterator<char>() };

	const auto firstResult = First(ParseInput(input));

	std::cout << "First " << to_string(firstResult) << std::endl; // 85726502

	// const auto secondResult = Second(input);
	// std::cout << "Second " << secondResult << std::endl; // 92768399

	return EXIT_SUCCESS;
}
