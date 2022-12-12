#include "day.h"
#include "utils.h"

#include <string>
#include <span>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <numeric>
#include <iterator>

using namespace std;

class Rucksack {
    string contents;

public:
    Rucksack(const string input) :contents(input) {

    }

    char findCommonInCompartments() {
        auto begin = contents.begin();
        auto end = contents.end();
        auto mid = begin + std::distance(begin, end) / 2;

        span<char> compartment1(begin, mid);
        span<char> compartment2(mid, end);

        std::sort(compartment1.begin(), compartment1.end());
        std::sort(compartment2.begin(), compartment2.end());

        auto i1 = compartment1.begin();
        auto i2 = compartment2.begin();

        while (i1 != compartment1.end() && i2 != compartment2.end()) {
            if (*i1 == *i2) {
                return *i1;
            }
            if (*i1 < *i2) {
                ++i1;
            }
            else {
                ++i2;
            }
        }
        throw runtime_error("No common item was found");
    }


};

int itemPriority(const char ch) {
    if ('a' <= ch && ch <= 'z') {
        return ch - 'a' + 1;
    }
    else {
        return ch - 'A' + 27;
    }
}

char findCommon(string str1, string str2, string str3) {
    ranges::sort(str1);
    ranges::sort(str2);
    ranges::sort(str3);

    const auto [first1, last1] = ranges::unique(str1);
    const auto [first2, last2] = ranges::unique(str2);
    const auto [first3, last3] = ranges::unique(str3);

    vector<char> intermediate;
    set_intersection(str1.begin(), first1, str2.begin(), first2, back_inserter(intermediate));

    vector<char> result;
    set_intersection(intermediate.cbegin(), intermediate.cend(), str3.begin(), first3, back_inserter(result));

    if (result.size() != 1) {
        throw runtime_error("Bad result");
    }
    return result[0];
}

DayResult day3() {
    const auto lines = readFile("day_3_input.txt");

    const auto prioritySum = accumulate(lines.begin(), lines.end(), 0, [](int currentSum, const auto line) {
        Rucksack sack(line);
    return currentSum + itemPriority(sack.findCommonInCompartments());
        }
    );

    int badgePrioritySum = 0;
    auto iter = lines.begin();
    while (iter != lines.end()) {
        const auto badge = findCommon(*iter, *(iter + 1), *(iter + 2));
        const auto priority = itemPriority(badge);
        badgePrioritySum += priority;
        iter += 3;
    }

    return {
        make_optional<PartialDayResult>({"Priority sum", to_string(prioritySum)}),
        make_optional<PartialDayResult>({"Badge priority sum", to_string(badgePrioritySum)})
    };
}
