#include "utils.h"
#include <ranges>
#include <numeric>
#include <functional>

using namespace std;

const string test_raw_input = R"(...#......
.......#..
#.........
..........
......#...
.#........
.........#
..........
.......#..
#...#.....)";

utils::Grid<char> readGrid(const vector<string>& input) {
    utils::Grid<char> result;

    result.data.reserve(input.size());
    for(const auto& line: input) {
        vector<char> chars;
        chars.reserve(line.size());
        ranges::copy(line, back_inserter(chars));

        result.data.emplace_back(move(chars));
    }

    return result;
}

struct GalaxyPair{
    unsigned long gridDistance;
    unsigned long multiples;
    unsigned long emptySpaces;

    unsigned long computeDistance() const {
        return (gridDistance + emptySpaces)* multiples;
    }

    unsigned long computeDistance2() const {
        return (gridDistance + emptySpaces * (1000000 - 1))* multiples;
    }
};

pair<unsigned long, unsigned long> compute(const vector<string>& input) {
    const auto make_galaxy_pair = [] (auto fromIter, auto toIter) {
            GalaxyPair newPair;
            newPair.gridDistance = distance(fromIter, toIter);
            newPair.multiples = *fromIter * *toIter;
            newPair.emptySpaces = ranges::count(fromIter, toIter, 0);
            return newPair;
    };

    auto grid = readGrid(input);
    // horizontal computation is independent from vertical computation
    // project all galaxies to rows and columns
    vector<unsigned long> columnGalaxies;
    const auto width = grid.data.at(0).size();
    columnGalaxies.reserve(width);
    for(size_t x = 0; x < width; ++x) {
        const auto galaxiesAtX = transform_reduce(
            grid.data.begin(),
            grid.data.end(),
            0ul, plus<long>(),
            [x](const auto& line) {
                return line[x] == '#';
            });
        columnGalaxies.push_back(galaxiesAtX);
    }

    vector<GalaxyPair> pairs;
    for(auto fromGalaxyPos = columnGalaxies.begin(); fromGalaxyPos != columnGalaxies.end(); ++fromGalaxyPos) {
        if(!*fromGalaxyPos){ continue; }

        for(auto toGalaxyPos = next(fromGalaxyPos); toGalaxyPos != columnGalaxies.end(); ++toGalaxyPos) {
            if(!*toGalaxyPos){ continue; }

            pairs.emplace_back(make_galaxy_pair(fromGalaxyPos, toGalaxyPos));
        }
    }
        
    //----------

    vector<unsigned long> rowGalaxies;
    const auto height = grid.data.size();
    rowGalaxies.reserve(grid.data.size());
    for(size_t y = 0; y < height; ++y) {
        const auto galaxiesAtY = ranges::count(grid.data.at(y), '#');
        rowGalaxies.push_back(galaxiesAtY);
    }

    for(auto fromGalaxyPos = rowGalaxies.begin(); fromGalaxyPos != rowGalaxies.end(); ++fromGalaxyPos) {
        if(!*fromGalaxyPos){ continue; }

        for(auto toGalaxyPos = next(fromGalaxyPos); toGalaxyPos != rowGalaxies.end(); ++toGalaxyPos) {
            if(!*toGalaxyPos){ continue; }

            pairs.emplace_back(make_galaxy_pair(fromGalaxyPos, toGalaxyPos));
        }
    }

    const auto result1 = transform_reduce(
        cbegin(pairs),
        cend(pairs),
        0ul,
        plus<unsigned long>(),
        mem_fn(&GalaxyPair::computeDistance));

    const auto result2 = transform_reduce(
        cbegin(pairs),
        cend(pairs),
        0ul,
        plus<unsigned long>(),
        mem_fn(&GalaxyPair::computeDistance2));

    return make_pair(result1, result2);;
}

int main(const int, const char* []) {
    cout << "Day 11" << endl;

    const auto [first_test_result, second_test_result] = compute(utils::split(test_raw_input));
    assertEquals(374ul, first_test_result);
    cout << "Test ok" << endl;

    const auto input = utils::readFile("input/day11");
    const auto [first_result, second_result]  = compute(input);
    cout << "First: " << first_result << endl;

    // Skip second aytomated test as the multiplier is different
    // assertEquals(8410ul, second_test_result);
    // cout << "Test 2 ok" << endl;
    cout << "Second: " << second_result << endl;

    return EXIT_SUCCESS;
}
