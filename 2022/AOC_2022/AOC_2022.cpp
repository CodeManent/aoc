#include "day.h"

#include <vector>
#include <iostream>

using namespace std;

int main() {
    vector<DayResult(*)()> days{
        day1,
        day2,
        day3,
        day4,
        day5,
        day6,
        day7,
        day8,
        day9,
        day10,
    };

    for (int i = 0; i < days.size(); ++i) {
        cout << "Day " << i + 1 << endl;
        const auto [partial1, partial2] = days.at(i)();
        if (partial1.has_value()) {
            cout << partial1->description << ": " << partial1->value << endl;
        }

        if (partial2.has_value()) {
            cout << partial2->description << ": " << partial2->value << endl;
        }
        cout << endl;
    }

    return 0;
}
