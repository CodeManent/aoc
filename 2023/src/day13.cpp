#include "utils.h"
#include <optional>
#include <numeric>
#include <ranges>

using namespace std;

const string test_raw_input = R"(#.##..##.
..#.##.#.
##......#
##......#
..#.##.#.
..##..##.
#.#.##.#.

#...##..#
#....#..#
..##..###
#####.##.
#####.##.
..##..###
#....#..#)";

vector<utils::Grid<char>> readGrids(const vector<string>& input) {    
    auto res = input | views::split(""s) | views::transform([](auto subRange) {
        auto innerRes = subRange | views::transform([](const string& str) {
            return vector<char>{begin(str), end(str)};
        });

        return utils::Grid<char>{vector<vector<char>>{begin(innerRes), end(innerRes)}};
    });

    return vector<utils::Grid<char>>(begin(res), end(res));
}

optional<size_t> findMirrorRow(auto& input){
    auto getMirrorPoint = [&input] (const size_t yTop, const size_t yBottom) -> optional<size_t> {
        // cout << "Checking mirror for " << make_pair(yTop, yBottom) << endl;

        const auto mid = midpoint(yTop, yBottom);
        const auto halfMirrorSize = mid - yTop + 1;

        auto mirrorRows = input | views::drop(yTop);

        auto firstHalf  = mirrorRows | views::take(halfMirrorSize);
        auto secondHalf = mirrorRows | views::drop(halfMirrorSize) | views::take(halfMirrorSize) | views::reverse;

        if(ranges::mismatch(firstHalf, secondHalf).in1 == end(firstHalf)) {
            // cout << "Found on " << mid << endl;
            return make_optional(mid + 1); // +1 because problem description has 1-based row numbers
        }
        else{
            return nullopt;
        }
    };

    const bool evenRowCount = (input.size() % 2) == 0;
    if(evenRowCount){
        throw runtime_error("Input data doesn't have an even-row count grid. Even row count grid was encountered.");;
    }

    { // check that the top edge is mirrored
        size_t yTop = 0;
        size_t maxYBottom = input.size() - (evenRowCount ? 0 : 1) - 1;
        for(size_t yBottom = maxYBottom; yBottom != -1ul ; yBottom -= 2) {
            if(const auto mirrorPoint = getMirrorPoint(yTop, yBottom); mirrorPoint.has_value()){
                return mirrorPoint;
            }
        }
    }

    { // check that bottom edge is mirrored
        size_t yBottom = input.size() - 1;
        for(size_t yTop = 1; yTop < yBottom; yTop += 2) {
            if(const auto mirrorPoint = getMirrorPoint(yTop, yBottom); mirrorPoint.has_value()){
                return mirrorPoint;
            }
        }
    }

    return nullopt;
}

optional<size_t> findHorizontalMirrorLocation(const utils::Grid<char> grid) {
    // cout << "Checking horizontal" << endl;
    return findMirrorRow(grid.data);
}

vector<vector<char>> transpose(const vector<vector<char>>& input) {
    vector<vector<char>> transposed;
    transposed.reserve(input.front().size());
    for(size_t x = 0; x < input.front().size(); ++x){
        vector<char> v;
        v.reserve(input.size());
        for(size_t y = 0; y < input.size(); ++y) {
            v.push_back(input.at(y).at(x));
        }
        transposed.emplace_back(move(v));
    }

    return transposed;
}

optional<size_t> findVerticalMirrorLocation(const utils::Grid<char> grid) {
    // instead of creating a transposed grid, we should add proper iterators or accessors to the grid
    // good enough for now
    const auto transposed = transpose(grid.data);

    return findMirrorRow(transposed);
}

long first(const vector<string>& input) {
    const auto grids = readGrids(input);

    vector<optional<size_t>> hMirrorPositions;
    vector<optional<size_t>> vMirrorPositions;

    for(const auto& grid: grids) {
        vMirrorPositions.emplace_back(findVerticalMirrorLocation(grid));
        if(vMirrorPositions.back().has_value()){
            hMirrorPositions.emplace_back(nullopt);
        }
        else{
            hMirrorPositions.emplace_back(findHorizontalMirrorLocation(grid));
        }    
    }

    const auto vResult = transform_reduce(
        cbegin(vMirrorPositions), cend(vMirrorPositions),
        0ul, plus<unsigned long>(),
        [] (const optional<unsigned long> optVPos) {
            return optVPos.value_or(0);
        });

    const auto result = transform_reduce(
        cbegin(hMirrorPositions), cend(hMirrorPositions),
        vResult, plus<>(),
        [] (const optional<unsigned long> optHPos){
            return optHPos.value_or(0) * 100;
        });

    return result;
}

// long second(const vector<string>& input) {
//     return input.size();
// }

int main(const int, const char* []) {
    cout << "Day 13" << endl;

    const auto first_test_result = first(utils::split(test_raw_input));
    assertEquals(405, first_test_result);
    cout << "Test ok" << endl;

    const auto input = utils::readFile("input/day13");
    const auto first_result = first(input);
    assertNotEquals(23558, first_result);
    assertEquals(36448, first_result);
    cout << "First: " << first_result << endl;

    // const auto second_test_result = second(utils::split(test_raw_input));
    // assertEquals(4, second_test_result);
    // cout << "Test 2 ok" << endl;

    // const auto second_result = second(input);
    // cout << "Second: " << second_result << endl;

    return EXIT_SUCCESS;
}
