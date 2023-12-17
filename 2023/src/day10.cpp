#include "utils.h"
#include "Point.h"

#include <sstream>
#include <memory>
#include <numeric>
#include <ranges>
#include <memory>
#include <functional>

using namespace std;

const string test_raw_input = R"(.....
.S-7.
.|.|.
.L-J.
.....)";

enum class Tile: char{
    VPipe = '|',
    HPipe = '-',
    NEBend = 'L',
    NWBend = 'J',
    SWBend = '7',
    SEBend = 'F',
    Ground = '.',
    Start = 'S',
    Deleted = ' '
};

ostream& operator<<(ostream& os, const Tile& tile) {
    switch(tile) {
    case Tile::VPipe:  return os << "│";
    case Tile::HPipe:  return os << "─";
    case Tile::NEBend: return os << "└";
    case Tile::NWBend: return os << "┘";
    case Tile::SWBend: return os << "┐";
    case Tile::SEBend: return os << "┌";
    case Tile::Ground: return os << ".";
    case Tile::Start:  return os << "╬";
    case Tile::Deleted: return os << " ";
    default:
        throw runtime_error("Unhandled Tile case on operator<<");
    }
}

vector<decltype(mem_fn(&Point::up))> getExpanders(const Tile& tile) {
    using enum Tile;

    switch(tile) {
    case VPipe  : return { mem_fn(&Point::up),   mem_fn(&Point::down) };
    case HPipe  : return { mem_fn(&Point::left), mem_fn(&Point::right) };
    case NEBend : return { mem_fn(&Point::up),   mem_fn(&Point::right) };
    case NWBend : return { mem_fn(&Point::up),   mem_fn(&Point::left) };
    case SWBend : return { mem_fn(&Point::down), mem_fn(&Point::left) };
    case SEBend : return { mem_fn(&Point::down), mem_fn(&Point::right) };
    case Ground : return {}; // empty
    case Deleted: return {};
    case Start  : return {
        mem_fn(&Point::up),
        mem_fn(&Point::down),
        mem_fn(&Point::left),
        mem_fn(&Point::right)
    };
    default:
        throw runtime_error("Unhandled Tile case on getExpanders: " + string({static_cast<char>(tile)}));
    }
}

vector<Point> expand(const Point& from, const Tile& tile){
    vector<Point> expanded;
    for(auto& expander: getExpanders(tile)){
        expanded.emplace_back(expander(from));
    }
    return expanded;
}

struct Context{
    vector<vector<Tile>> entry;
};

vector<Point> expand(const Context& context, const Point&point) {
    const auto tile = context.entry.at(point.y).at(point.x);
    vector expanded = expand(point, tile);
    /// remove out-of-map-bounds points
    expanded.erase(
        remove_if(
            begin(expanded),
            end(expanded),
            [&context](const Point& p) {
                return p.x < 0 
                    || p.y < 0
                    || p.x >= static_cast<long>(context.entry.front().size())
                    || p.y >= static_cast<long>(context.entry.size());
            }),
        end(expanded));

    return expanded;
}

Context parseInput(const vector<string>& input) {
    Context result;

    for(const auto& line: input){
        vector<Tile> entries;
        entries.reserve(line.size());
        std::transform(line.cbegin(), line.cend(), back_inserter(entries), [](const char& ch) {
            return static_cast<Tile>(ch);
        });

        result.entry.emplace_back(move(entries));
    }

    return result;
}

Point findStart(const Context& context){
    for(long y = 0; const auto& line : context.entry) {
        for(long x = 0; const auto& entry: line) {
            if(entry == Tile::Start) {
                return Point{x, y};
            }
            ++x;
        }
        ++y;
    }

    throw logic_error("Start not found");
}

template <typename Iter, typename Iter2>
bool beginsWith(Iter at, Iter end, Iter2 with, Iter2 withEnd) {
    for(; at != end && with != withEnd; ++with, ++at) {
        if(*at != *with) {
            return false;
        }
    }

    return with == withEnd;
}

const vector<pair<vector<Tile>, vector<Tile>>> replacements = {
    {{Tile::VPipe,  Tile::SEBend, Tile::SWBend, Tile::VPipe}, {Tile::SEBend, Tile::Deleted, Tile::Deleted, Tile::SWBend}},
    {{Tile::VPipe,  Tile::SEBend, Tile::SWBend, Tile::NEBend},{Tile::SEBend, Tile::Deleted, Tile::Deleted, Tile::HPipe}},
    {{Tile::NWBend, Tile::SEBend, Tile::SWBend, Tile::VPipe}, {Tile::HPipe,  Tile::Deleted, Tile::Deleted, Tile::SWBend}},
    {{Tile::NWBend, Tile::SEBend, Tile::SWBend, Tile::NEBend},{Tile::HPipe,  Tile::Deleted, Tile::Deleted, Tile::HPipe}},

    {{Tile::VPipe,  Tile::NWBend, Tile::NEBend, Tile::VPipe}, {Tile::NWBend, Tile::Deleted, Tile::Deleted, Tile::NEBend}},
    {{Tile::SEBend, Tile::NWBend, Tile::NEBend, Tile::VPipe}, {Tile::HPipe,  Tile::Deleted, Tile::Deleted, Tile::NEBend}},
    {{Tile::VPipe,  Tile::NWBend, Tile::NEBend, Tile::SWBend},{Tile::NWBend, Tile::Deleted, Tile::Deleted, Tile::HPipe}},
    {{Tile::SEBend, Tile::NWBend, Tile::NEBend, Tile::SWBend},{Tile::HPipe,  Tile::Deleted, Tile::Deleted, Tile::HPipe}},

    {{Tile::HPipe,  Tile::SWBend, Tile::NWBend, Tile::HPipe}, {Tile::SWBend, Tile::Deleted, Tile::Deleted, Tile::NWBend}},
    {{Tile::NEBend, Tile::SWBend, Tile::NWBend, Tile::HPipe}, {Tile::VPipe,  Tile::Deleted, Tile::Deleted, Tile::NWBend}},
    {{Tile::HPipe,  Tile::SWBend, Tile::NWBend, Tile::SEBend},{Tile::SWBend, Tile::Deleted, Tile::Deleted, Tile::VPipe}},
    {{Tile::NEBend, Tile::SWBend, Tile::NWBend, Tile::SEBend},{Tile::VPipe,  Tile::Deleted, Tile::Deleted, Tile::VPipe}},

    {{Tile::HPipe,  Tile::NEBend, Tile::SEBend, Tile::HPipe}, {Tile::NEBend, Tile::Deleted, Tile::Deleted, Tile::SEBend}},
    {{Tile::SWBend, Tile::NEBend, Tile::SEBend, Tile::HPipe}, {Tile::VPipe,  Tile::Deleted, Tile::Deleted, Tile::SEBend}},
    {{Tile::HPipe,  Tile::NEBend, Tile::SEBend, Tile::NWBend},{Tile::NEBend, Tile::Deleted, Tile::Deleted, Tile::VPipe}},
    {{Tile::SWBend, Tile::NEBend, Tile::SEBend, Tile::NWBend},{Tile::VPipe,  Tile::Deleted, Tile::Deleted, Tile::VPipe}},
};

bool simplify(vector<vector<Tile>>& grid, vector<Point>& path, bool clockwise){
    using enum Tile;

    // transform vector of points to vector of symbols
    vector<Tile> symbols;
    symbols.reserve(path.size());
    transform(cbegin(path), cend(path), back_inserter(symbols), [&grid](const Point& p){
        return grid.at(p.y).at(p.x);
    });

    bool ruleApplied = false;

    for(size_t i=0; i< symbols.size(); ++i) {
        // check if there is a match for any rule in the position
        auto matchPosition = begin(symbols) + i;
        for(const auto& rule: replacements) {
            if(clockwise && beginsWith(matchPosition, end(symbols), begin(rule.first), end(rule.first))) {
                auto pathIter = path.begin() + i;
                for(auto changeWithIter = begin(rule.second); changeWithIter != end(rule.second); ++changeWithIter, ++pathIter) {
                    const auto& pos = *pathIter;
                    grid.at(pos.y).at(pos.x) = *changeWithIter;
                }
                i += rule.second.size();

                ruleApplied = true;
                goto skip_other_rules;
            }
            else if(!clockwise && beginsWith(matchPosition, end(symbols), rbegin(rule.first), rend(rule.first))) {
                // reverse rule
                auto pathIter = path.begin() + i;
                for(auto changeWithIter = rbegin(rule.second); changeWithIter != rend(rule.second); ++changeWithIter, ++pathIter) {
                    const auto& pos = *pathIter;
                    grid.at(pos.y).at(pos.x) = *changeWithIter;
                }
                i += rule.second.size();

                ruleApplied = true;
                goto skip_other_rules;
            }
        }
        skip_other_rules:;
    }

    // trim path
    path.erase(remove_if(path.begin(), path.end(), [&grid](const Point& p) {
        return grid.at(p.y).at(p.x) == Tile::Deleted;
    }), path.end());

    return ruleApplied;
}


pair<long, long> compute(const vector<string>& input) {
    auto context = parseInput(input);

    // print(context.entry);

    const auto start = findStart(context);

    // find connected points to start
    auto connectedToStart = expand(context, start);

    connectedToStart.erase(
        remove_if(
            begin(connectedToStart),
            end(connectedToStart),
            [&start, &context](const Point& p) {
                return ranges::all_of(expand(context, p), not_fn(is_equal_to(start)));
            }),
            end(connectedToStart));


    // print(connectedToStart);
    assertEquals(2ul, connectedToStart.size(), "There should be two points connected to start");

    // expand one of the pipes until wee loop back to start
    vector<Point> pipe {start, connectedToStart[0]};
    while(pipe.back() != start) {
        const auto current = pipe.back();
        const auto previous = *prev(prev(pipe.end()));
        auto frontier = expand(context, current);
        // cout << "previous: " << previous << ", current: " << current << endl;
        // print(frontier);
        frontier.erase(remove(begin(frontier), end(frontier), previous), end(frontier));
        // print(frontier);
        assertEquals(1ul, frontier.size(), "There should be only one next pipe node");

        const auto next = frontier[0];
        // cout << "current: " << current << ", next: " << next << endl;
        pipe.emplace_back(next);
    }

    auto result1 = pipe.size() / 2;

    // part2
    vector<vector<Tile>> onlyPipe;
    for(const auto& entry: context.entry){
        onlyPipe.emplace_back(vector(entry.size(), Tile::Ground));
    }
    for(const Point& point: pipe) {
        onlyPipe.at(point.y).at(point.x) = context.entry.at(point.y).at(point.x);
    }
    // auto printOnlyPipe = [&onlyPipe]() {
    //     for(const auto& line: onlyPipe) {
    //         for(const Tile& tile: line) {
    //             cout << tile;
    //         }
    //         cout << endl;
    //     }
    // };

    // print(replacements);

    // pull back the slim branches
    for(size_t i = 1 ; simplify(onlyPipe, pipe, true); ++i) {
        // printOnlyPipe();
        // cout << "Iteration " << i << endl;
    }

    // bloat it up a bit so it won't be that visually sharp
    for(size_t i = 1 ; simplify(onlyPipe, pipe, false); ++i) {
        // printOnlyPipe();
        // cout << "Backwards Iteration " << i << endl;
    }

    // cleanup exterior. A flood fill should be better but this is enough
    for(auto& line: onlyPipe) {
        for(auto& tile : line) {
            // clear front
            if(tile != Tile::Ground && tile != Tile::Deleted) {
                break;
            }
            tile = Tile::Deleted;
        }

        //clear back
        for(auto& tile: views::reverse(line)) {
            if(tile != Tile::Ground && tile != Tile::Deleted) {
                break;
            }
            tile = Tile::Deleted;
        }
    }

    auto containedSpaces = ranges::count_if(
        onlyPipe | views::join,
        [](const Tile& tile) {
            return tile == Tile::Ground;
        });

    return make_pair(result1, containedSpaces);
}

int main(const int, const char* []) {
    cout << "Day 10" << endl;

    const auto [first_test_result, second_test_result] = compute(utils::split(test_raw_input));
    assertEquals(4, first_test_result);
    cout << "Test ok" << endl;

    const auto input = utils::readFile("input/day10");
    const auto [first_result, second_result] = compute(input);
    cout << "First: " << first_result << endl;

    assertEquals(1, second_test_result);
    cout << "Test 2 ok" << endl;

    cout << "Second: " << second_result << endl;

    return EXIT_SUCCESS;
}
