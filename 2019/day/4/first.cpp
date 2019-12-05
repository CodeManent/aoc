#include <iostream>
#include <cstdlib>
#include <vector>
#include <tuple>
#include <algorithm>

const int powers[] = {
	0,
	1,
	10,
	100,
	1000,
	10000,
	100000,
	1000000
};
const char passChars[] = "0123456789";

constexpr void SetPass(const int value, char pass[6]) {
	for (int i = 0; i < 6; ++i)	{
		const int index = (value / powers[6-i]) % 10;
		pass[i] = passChars[index];
	}
}

template <int size>
constexpr int PassToInt(const char pass[size]){

	return (pass[0] - '0') * powers[size] + PassToInt<size - 1>(pass + 1);
}

template <>
constexpr int PassToInt<0>(const char pass[0])
{
	return 0;
}

bool IsValidPassword(const char pass[6]) {
	int numPass = PassToInt<6>(pass);

	bool foundPair = false;
	for (int i = 0; i < 5; ++i) {
		foundPair = foundPair || (pass[i] == pass[i+1]);
		if(pass[i] > pass[i+1]){
			return false;
		}
	}

	return foundPair;
}

int CountPasswords() {
	int minVal = 272091;
	int maxVal = 815432;

	char pass[6];

	int counter = 0;

	for(int val = minVal; val <= maxVal; ++val) {
		SetPass(val, pass);
		if(IsValidPassword(pass)){
			++counter;
		}
	}

	return counter;
}


bool Test() {
	std::vector<std::tuple<bool, int, const char*>> testCases = {
		{false, 123456, "123456"},
		{true , 111111, "111111"},
		{false, 223450, "223450"},
		{false, 123789, "123789"},
		{true , 123788, "123788"}};

	for(const auto &tc: testCases) {
		const char *strPass = std::get<2>(tc);
		int intPass = PassToInt<6>(strPass);
		if (intPass != std::get<1>(tc)) {
			std::cerr << "PassToInt faild for \"" << std::get<2>(tc) << "\" with value " << intPass << std::endl;
				return false;
		}

		char buffer[7] = {'0'};
		SetPass(intPass, buffer);
		for(int i = 0 ; i < 6; ++i) {
			if(buffer[i] != strPass[i]) {
				std::cerr << "Bas SetPass(" << intPass << ") = " << buffer << std::endl;
				return false;
			}
		}


		bool isValid = IsValidPassword(std::get<2>(tc));

		if(isValid != std::get<0>(tc) ){
			std::cerr << "Bab validation of " << std::get<2>(tc) << std::endl;
			return false;
		}
	}

	return true;
}

int main(int,char**) {
	std::cout << "Day 4, First" << std::endl;

	if(!Test()){
		return EXIT_FAILURE;
	}

	std::cout << CountPasswords() << std::endl;

	return EXIT_SUCCESS;
}
