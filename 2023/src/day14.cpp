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

template <typename TIter>
void tiltElementLogic(Platform &result, TIter &elementIt, TIter &lastEmpty){
    const auto element = *elementIt;
    switch(element) {
        case Tile::Empty:
            result.at(elementIt.p) = Tile::Empty;
            break;

        case Tile::CubeRock:
            result.at(elementIt.p) = Tile::CubeRock;
            lastEmpty = elementIt;
            ++lastEmpty;
            break;
        
        case Tile::RoundRock:
            result.at(elementIt.p) = Tile::Empty;
            result.at(lastEmpty.p) = Tile::RoundRock;
            ++lastEmpty;
            break;
    }
}

Platform tiltUp(const Platform& in) {
    Platform result;
    result.grow(in.data.front().size(), in.data.size(), Tile::Empty);

    for(auto columnIt = in.beginColumns(); columnIt != in.endColumns(); ++columnIt) {
        for(auto elementIt = (*columnIt).begin(), lastEmpty = elementIt; elementIt != (*columnIt).end(); ++elementIt) {
            tiltElementLogic(result, elementIt, lastEmpty);
        }
    }

    return result;
}

Platform tiltDown(const Platform& in){
    Platform result;
    result.grow(in.data.front().size(), in.data.size(), Tile::Empty);

    for(auto columnIt = in.beginColumns(); columnIt != in.endColumns(); ++columnIt) {
        for(auto elementIt = (*columnIt).rbegin(), lastEmpty = elementIt; elementIt != (*columnIt).rend(); ++elementIt) {
            tiltElementLogic(result, elementIt, lastEmpty);
        }
    }

    return result;
}

Platform tiltLeft(const Platform& in){
    Platform result;
    result.grow(in.data.front().size(), in.data.size(), Tile::Empty);

    for(auto rowIt = in.beginRows(); rowIt != in.endRows(); ++rowIt) {
        for(auto elementIt = (*rowIt).begin(), lastEmpty = elementIt; elementIt != (*rowIt).end(); ++elementIt) {
            tiltElementLogic(result, elementIt, lastEmpty);
        }
    }

    return result;
}

Platform tiltRight(const Platform& in){
    Platform result;
    result.grow(in.data.front().size(), in.data.size(), Tile::Empty);

    for(auto rowIt = in.beginRows(); rowIt != in.endRows(); ++rowIt) {
        for(auto elementIt = (*rowIt).rbegin(), lastEmpty = elementIt; elementIt != (*rowIt).rend(); ++elementIt) {
            tiltElementLogic(result, elementIt, lastEmpty);
        }
    }

    return result;
}

size_t computeLoad(const Platform& platform) {
    const size_t height = platform.data.size();
    size_t result = 0;
    for(auto columnIt = platform.beginColumns(); columnIt != platform.endColumns(); ++columnIt) {
        size_t rowScore = height;
        for(const auto element : *columnIt) {
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
