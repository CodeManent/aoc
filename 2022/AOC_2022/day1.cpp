#include "day.h"
#include "utils.h"

#include <ranges>
#include <algorithm>
#include <numeric>
#include <vector>
#include <string>

using namespace std;

vector<vector<string>> toGroups(vector<string> input) {
    vector<vector<string>> result;

    auto beginIter = input.begin();
    auto currentIter = beginIter;
    while (++currentIter != input.end()) {
        if (currentIter->empty()) {
            // found the empty line;
            result.push_back(vector<string>(beginIter, currentIter));
            beginIter = currentIter + 1;
        }
    }

    return result;
}

DayResult day1()
{
    const auto fileData = readFile("day_1_input.txt");

    const auto groups = toGroups(fileData);

    vector<vector<int>> intGroups;
    ranges::transform(
        groups,
        back_inserter(intGroups),
        [](const auto& strGroup) {
            vector<int> localResult;
    ranges::transform(
        strGroup,
        back_inserter(localResult),
        [](const auto& str) {return stol(str); });

    return localResult;
        });

    vector<int> sums;
    ranges::transform(
        intGroups,
        back_inserter(sums),
        [](const auto& group) {
            return accumulate(group.begin(), group.end(), 0);
        }
    );

    const auto maxSum = std::ranges::max(sums);
    //std::cout << "Max sum: " << maxSum << endl;

    ranges::sort(sums, ranges::greater());
    // std::cout << sums << endl;

    const auto top3Sum = accumulate(sums.begin(), sums.begin() + 3, 0);
    //std::cout << "Sum of top 3: " << top3Sum << endl;

    return {
        make_optional<PartialDayResult>({"Max sum", to_string(maxSum)}),
        make_optional<PartialDayResult>({"Sum of top 3", to_string(top3Sum)})
    };
}
