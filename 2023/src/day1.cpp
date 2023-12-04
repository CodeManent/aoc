#include "utils.h"

#include <numeric>
#include <algorithm>
#include <iterator>
#include <utility>

using namespace std;

const string test_raw_input = R"(1abc2
pqr3stu8vwx
a1b2c3d4e5f
treb7uchet)";

const string digits = "123456789";
const vector<string> wordDigits {
    // "zero",
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine"
};

long first(const vector<string> input) {
    // print(input, "input");

    vector<pair<char, char>> pairs;
    transform(begin(input), end(input), back_inserter(pairs), [](const string& line) {
        const auto first = line.find_first_of(digits);
        const auto last = line.find_last_of(digits);
        return make_pair(line[first], line[last]);
    });
    // print(strNumbers, "strNumbers");

    long result = transform_reduce(
        cbegin(pairs),
        cend(pairs),
        0,
        plus<long>(),
        [](const pair<char, char>& charPair) {
            char num[3] = {
                charPair.first,
                charPair.second,
                '\0'
            };

            return stol(num);
        }
    );
    
    return result;
}

const string test_raw_input_2 = R"(two1nine
eightwothree
abcone2threexyz
xtwone3four
4nineeightseven2
zoneight234
7pqrstsixteen)";

struct FoundNumber{
    size_t pos = string::npos;
    long value = 0;
};

ostream& operator<< (ostream& os, const FoundNumber& found) {
    return os << "(pos: " << found.pos << ", value:" << found.value << ")";
}

const FoundNumber& selectFirstNumber(const FoundNumber& lhs, const FoundNumber& rhs) {
    if(lhs.pos == string::npos) {
        return rhs;
    }
    else if (rhs.pos == string::npos) {
        return lhs;
    }
    else{
        return lhs.pos <= rhs.pos ? lhs : rhs;
    }
    return rhs;
}

const FoundNumber& selectLastNumber(const FoundNumber& lhs, const FoundNumber& rhs) {
    if(lhs.pos == string::npos) {
        return rhs;
    }
    else if (rhs.pos == string::npos) {
        return lhs;
    }
    else{
        return lhs.pos >= rhs.pos ? lhs : rhs;
    }
    return rhs;
}

long second(const vector<string>& input) {
    // print(input);

    vector<pair<FoundNumber, FoundNumber>> pairs;

    transform(
        begin(input),
        end(input),
        back_inserter(pairs),
        [](const string& line) {
            const auto firstDigitPos = line.find_first_of(digits);
            const FoundNumber firstDigit {
                firstDigitPos,
                static_cast<long>(1 + digits.find(line[firstDigitPos]))};

            const auto lastDigitPos = line.find_last_of(digits);
            const FoundNumber lastDigit {
                lastDigitPos,
                static_cast<long>(1 + digits.find(line[lastDigitPos]))};

            const auto firstWordDigit = transform_reduce(
                cbegin(wordDigits),
                cend(wordDigits),
                FoundNumber{},
                selectFirstNumber,
                [&line](const string& word) {
                    const auto pos = line.find(word);
                    return FoundNumber{
                        pos,
                        // position in wordDigits is the value of the digit
                        1 + distance(
                            cbegin(wordDigits),
                            find(
                                cbegin(wordDigits),
                                cend(wordDigits),
                                word))};
                });

            const auto lastWordDigit = transform_reduce(
                cbegin(wordDigits),
                cend(wordDigits),
                FoundNumber{},
                selectLastNumber,
                [&line](const string& word) {
                    const auto pos = line.rfind(word);
                    return FoundNumber{
                        pos,
                        // position in wordDigits is the value of the digit
                        1 + distance(
                            cbegin(wordDigits),
                            find(
                                cbegin(wordDigits),
                                cend(wordDigits),
                                word))};
                });

            const auto first = selectFirstNumber(firstDigit, firstWordDigit);
            const auto last = selectLastNumber(lastDigit, lastWordDigit);

            return make_pair(first, last);
        });

    // for(size_t i = 0; i < input.size(); ++i) {
    //     const string before = input[i].substr(0, pairs[i].first.pos);
    //     const string middle = input[i].substr(pairs[i].first.pos, pairs[i].second.pos - pairs[i].first.pos);
    //     const string after = input[i].substr(pairs[i].second.pos);
    //     cout << i << ":\t" << pairs[i].first.value << " " <<  pairs[i].second.value << " - " << before << " " << middle << " " << after << "\n";
    // }

    // print(pairs);

    long result = transform_reduce(
        cbegin(pairs),
        cend(pairs),
        0,
        plus<long>(),
        [](const  auto& numberPair) {
            return numberPair.first.value * 10  + numberPair.second.value;
        });

        return result;
}

int main(const int, const char * const []) {
    std::cout << "Day 1" << endl;

    const auto first_test_result = first(utils::split(test_raw_input));
    assertEquals(142, first_test_result);
    std::cout << "Test ok" << endl;

    const auto input = utils::readFile("input/day1");
    // print(input);

    const auto first_result = first(input);
    cout << "First: " << first_result << endl;

    const auto second_test_result = second(utils::split(test_raw_input_2));
    assertEquals(281, second_test_result);
    cout << "Test 2 ok" << endl;

    const auto second_result = second(input);
    cout << "Second: " << second_result << endl;
    
    return EXIT_SUCCESS;
}
