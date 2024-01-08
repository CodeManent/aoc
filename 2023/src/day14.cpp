#include "utils.h"
#include <unordered_map>

using namespace std;

const string test_raw_input = R"(O....#....
O.OO#....#
.....##...
OO.#O....O
.O.....O#.
O.#..O.#.#
..O..#O..O
.......O..
#....###..
#OO..#....)";

const string tiltTest = R"(OOOO.#.O..
OO..#....#
OO..O##..O
O..#.OO...
........#.
..#....#.#
..O..#.O.O
..O.......
#....###..
#....#....)";

const string cycle1Input = R"(.....#....
....#...O#
...OO##...
.OO#......
.....OOO#.
.O#...O#.#
....O#....
......OOOO
#...O###..
#..OO#....)";

const string cycle2Input = R"(.....#....
....#...O#
.....##...
..O#......
.....OOO#.
.O#...O#.#
....O#...O
.......OOO
#..OO###..
#.OOO#...O)";

const string cycle3Input = R"(.....#....
....#...O#
.....##...
..O#......
.....OOO#.
.O#...O#.#
....O#...O
.......OOO
#...O###.O
#.OOO#...O)";

enum class Tile: char{
    Empty = '.',
    RoundRock = 'O',
    CubeRock = '#'
};

ostream& operator<< (ostream &os, const Tile &tile) {
    return os << static_cast<char>(tile);
}

using Platform = utils::Grid<Tile>;

Platform parseInput(const vector<string>& input) {
    Platform result;
    result.data.reserve(input.size());

    for(const auto& line: input) {
        auto tiles = line | views::transform([](const char ch) {
            switch(ch) {
                case '.': return Tile::Empty;
                case 'O': return Tile::RoundRock;
                case '#': return Tile::CubeRock;
                default:
                    throw runtime_error("Unexpected tile character: " + ch);
            }
        });

        result.data.emplace_back(begin(tiles), end(tiles));
    }

    return result;
}

template <typename TargetIterT, typename SourceIterT>
void tiltElementLogic(TargetIterT targetIt, SourceIterT elementIt, TargetIterT &lastEmpty){
    const auto element = *elementIt;

    switch(element) {
        case Tile::Empty:
            *targetIt = Tile::Empty;
            break;

        case Tile::CubeRock:
            *targetIt = Tile::CubeRock;
            lastEmpty = next(targetIt);
            break;
        
        case Tile::RoundRock:
            *targetIt = Tile::Empty;
            *lastEmpty = Tile::RoundRock;
            ++lastEmpty;
            break;
    }
}

Platform tiltUp(const Platform& in) {
    Platform result;
    result.grow(in.data.front().size(), in.data.size(), Tile::Empty);

    auto targetColumnIt = result.beginColumns();
    for(const auto& column: in.columns()) {
        auto targetIt = (*targetColumnIt).begin();
        auto lastEmpty = targetIt;
        for(auto sourceIt = column.begin(); sourceIt != column.end(); ++sourceIt, ++targetIt) {
            tiltElementLogic(targetIt, sourceIt, lastEmpty);
        }
        ++targetColumnIt;
    }

    return result;
}

Platform tiltDown(const Platform& in){
    Platform result;
    result.grow(in.data.front().size(), in.data.size(), Tile::Empty);

    auto targetColumnIt = result.beginColumns();
    for(const auto& column: in.columns()) {
        auto targetIt = (*targetColumnIt).rbegin();
        auto lastEmpty = targetIt;
        for(auto sourceIt = column.rbegin(); sourceIt != column.rend(); ++sourceIt, ++targetIt) {
            tiltElementLogic(targetIt, sourceIt, lastEmpty);
        }
        ++targetColumnIt;
    }

    return result;
}

Platform tiltLeft(const Platform& in){
    Platform result;
    result.grow(in.data.front().size(), in.data.size(), Tile::Empty);

    auto targetRowIt = result.beginRows();
    for(const auto& row: in.rows()) {
        auto targetIt = (*targetRowIt).begin();
        auto lastEmpty = targetIt;
        for(auto sourceIt = row.begin(); sourceIt != row.end(); ++sourceIt, ++targetIt) {
            tiltElementLogic(targetIt, sourceIt, lastEmpty);
        }
        ++targetRowIt;
    }

    return result;
}

Platform tiltRight(const Platform& in){
    Platform result;
    result.grow(in.data.front().size(), in.data.size(), Tile::Empty);

    auto targetRowIt = result.beginRows();
    for(const auto& row: in.rows()) {
        auto targetIt = (*targetRowIt).rbegin();
        auto lastEmpty = targetIt;
        for(auto sourceIt = row.rbegin(); sourceIt != row.rend(); ++sourceIt, ++targetIt) {
            tiltElementLogic(targetIt, sourceIt, lastEmpty);
        }
        ++targetRowIt;
    }

    return result;
}

size_t computeLoad(const Platform& platform) {
    const size_t height = platform.data.size();
    size_t result = 0;
    for(const auto& column: platform.columns()) {
        size_t rowScore = height;
        for(const auto element : column) {
            if(element == Tile::RoundRock){
                result += rowScore;
            }
            --rowScore;
        }
    }

    return result;
}

long first(const vector<string>& input) {
    const auto platform = parseInput(input);

    const auto tilted = tiltUp(platform);

    return computeLoad(tilted);
}

Platform cycle(const Platform& platform) {
    return tiltRight(tiltDown(tiltLeft(tiltUp(platform))));
}

long second(const vector<string>& input) {
    size_t cycles = 1000000000;
    auto platform = parseInput(input);

    auto customHasher = [](const Platform& p) {
        return computeLoad(p);
    };

    unordered_map<Platform, size_t, decltype(customHasher)> visited;

    bool foundLoop = false;
    for(size_t i = 1; i <= cycles; ++i) {
        platform = cycle(platform);
        if(!foundLoop){
            if(auto visitedIter = visited.find(platform); visitedIter == visited.end()) {
                visited.insert({platform, i});
            }
            else{
                assertEquals(visitedIter->first, platform);

                foundLoop = true;
                // found
                const auto loopStart = visitedIter->second;
                // cout << "Found loop between " << loopStart << " and " << i << endl;
                const auto loopSize = i - loopStart;
                // cout << "loop size: " << loopSize << endl;

                // const auto neededIterations = loopStart + ((cycles - loopStart) % loopSize);
                // cout << "Needed iterations: " << neededIterations << endl;

                const auto iterationsToTarget = i + (cycles - i) %loopSize;
                // cout << "Iterations to target: " << iterationsToTarget << endl;

                cycles = iterationsToTarget;
            }
        }
    }

    return computeLoad(platform);
}

int main(const int, const char* []) {
    cout << "Day 14" << endl;

    const auto first_test_result = first(utils::split(test_raw_input));
    assertEquals(136, first_test_result);
    cout << "Test ok" << endl;

    const auto input = utils::readFile("input/day14");
    const auto first_result = first(input);
    cout << "First: " << first_result << endl;

    auto testPlatform = parseInput(utils::split(test_raw_input));
    const auto cycle1Platform = parseInput(utils::split(cycle1Input));
    const auto cycle2Platform = parseInput(utils::split(cycle2Input));
    const auto cycle3Platform = parseInput(utils::split(cycle3Input));

    assertEquals(cycle1Platform, cycle(testPlatform));
    assertEquals(cycle2Platform, cycle(cycle(testPlatform)));
    assertEquals(cycle3Platform, cycle(cycle(cycle(testPlatform))));

    const auto second_test_result = second(utils::split(test_raw_input));
    assertEquals(64, second_test_result);
    cout << "Test 2 ok" << endl;

    const auto second_result = second(input);
    assertNotEquals(112433, second_result, "Too low");
    cout << "Second: " << second_result << endl;

    return EXIT_SUCCESS;
}
