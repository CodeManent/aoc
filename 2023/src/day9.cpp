#include "utils.h"
#include <sstream>
#include <memory>
#include <numeric>

using namespace std;

const string test_raw_input = R"(0 3 6 9 12 15
1 3 6 10 15 21
10 13 16 21 30 45)";

struct ValueSeries{
    vector<vector<long>> values;
};

struct Context{
    vector<ValueSeries> readings;
};

Context parseInput(const vector<string>& input) {
    Context result;

    for(const string& line: input) {
        vector<long> numbers;
        istringstream iss(line);
        iss >> numbers;

        ValueSeries newReading;
        newReading.values.emplace_back(move(numbers));

        result.readings.emplace_back(move(newReading));
    }

    return result;
}

pair<long, long> compute(const vector<string>& input) {
    auto context = parseInput(input);

    for(auto& reading: context.readings){
        while(!ranges::all_of(reading.values.back(), [](const auto& val){ return val == 0;})){
            const auto& from = reading.values.back();
            if(from.size() < 2) {
                throw new runtime_error("Need at least two values to compute a difference");
            }
            vector<long> diff;
            diff.reserve(from.size()-1);
            for(auto i = from.begin(), i2 = next(from.begin()); i2 != from.end(); ++i, ++i2){
                diff.emplace_back(*i2 - *i);
            }

            // cout << "from : " << from << endl;
            // cout << "diff : " << diff << endl;

            reading.values.emplace_back(move(diff));
        }

        // print(reading.values);
    }

    // expand
    for(auto& reading: context.readings){
        reading.values.back().emplace_back(0);
        for(auto i = next(rbegin(reading.values)); i != rend(reading.values); ++i) {
            i->emplace_back(i->back() + (i-1)->back());
        }

        // print(reading.values);
    }

    const auto result1 = transform_reduce(begin(context.readings), end(context.readings), 0, plus<long>(), [](const ValueSeries& series) {
        // the last element of the first vector
        return series.values.front().back();
    });

    // expand front
    for(auto& reading: context.readings){
        // add element to the front
        reading.values.back().insert(reading.values.back().begin(), 0);

        for(auto i = next(rbegin(reading.values)); i != rend(reading.values); ++i) {
            const auto newValue = i->front() - (i-1)->front();
            i->insert(i->begin(), newValue);
        }

        // print(reading.values);
    }

    const auto result2 = transform_reduce(begin(context.readings), end(context.readings), 0, plus<long>(), [](const ValueSeries& series) {
        // the first element of the first vector
        return series.values.front().front();
    });

    return make_pair(result1, result2);
}

int main(const int, const char* []) {
    cout << "Day 9" << endl;

    const auto [first_test_result, second_test_result] = compute(utils::split(test_raw_input));
    assertEquals(114, first_test_result);
    cout << "Test ok" << endl;

    const auto input = utils::readFile("input/day9");
    const auto [first_result, second_result] = compute(input);
    cout << "First: " << first_result << endl;

    assertEquals(2, second_test_result);
    cout << "Test 2 ok" << endl;

    cout << "Second: " << second_result << endl;

    return EXIT_SUCCESS;
}
