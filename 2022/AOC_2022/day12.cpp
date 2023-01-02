#include "day.h"
#include "utils.h"

#include "Coords.h"
#include "printers.h"

#include <iostream>
#include <unordered_set>
#include <queue>
#include <stdexcept>
#include <map>
#include <unordered_map>
#include <utility>
#include <set>
#include <type_traits>
#include <algorithm>
#include <functional>

using namespace std;

bool trace = false;

enum class Direction {
    Up,
    Down,
    Left,
    Right
};

Coords to_Coords(const Direction& d) {
    switch (d) {
    case Direction::Up: return Up;
    case Direction::Down: return Down;
    case Direction::Left: return Left;
    case Direction::Right: return Right;
    }
}

Direction to_Direction(const Coords& c) {
    if (c == Up) {
        return Direction::Up;
    }
    else if (c == Down) {
        return Direction::Down;
    }
    else if (c == Left) {
        return Direction::Left;
    }
    else if (c == Right) {
        return Direction::Right;
    }
}


class ElevationGrid {
public:
    vector<vector<char>> elevation;
    Coords start;
    Coords goal;
    bool forwardSearch = true;

    ElevationGrid(vector<string> input) {
        elevation.reserve(input.size());

        int y = 0;
        int x = 0;

        for (const auto line : input) {
            vector<char> row;
            row.reserve(line.length());

            for (const auto ch : line) {
                if (ch == 'S') [[unlikely]] {
                    start = Coords(x, y);
                    row.push_back('a');
                }
                else if (ch == 'E') [[unlikely]] {
                    goal = Coords(x, y);
                    row.push_back('z');
                }
                else{
                    row.push_back(ch); 
                }

                ++x;
            }
            elevation.push_back(move(row));
            ++y;
            x = 0;
        }
    }

    auto at(const Coords& pos) const {
        return elevation.at(pos.y).at(pos.x);
    }

    bool isValid(const Coords& pos) const {
        return 0 <= pos.y && pos.y < elevation.size() && 0 <= pos.x && pos.x < elevation.at(pos.y).size();
    }

    virtual bool allowTransition(const Coords& from, const Direction dir) const {
        if (!isValid(from)) {
            return false;
        }
        const auto elevationFrom = at(from);

        const auto to = from + to_Coords(dir);
        if (!isValid(to)) {
            return false;
        }
        const auto elevationTo = at(to);

        if (forwardSearch) {
            const auto d = elevationTo - elevationFrom;
            return d <= 1;
        }
        else {
            const auto d = elevationFrom - elevationTo;
            return d <= 1;
        }
    }
};


class Step {
public:
    Coords location;
    Direction dir;
};

class Path {
    vector<Coords> steps;
public:

    Path(const Coords& head)
        :steps{ head }
    {
    }

    auto head() const {
        return steps.back();
    }

    auto tail() const {
        return steps.back();
    }

    auto length() const {
        return steps.size() - 1;
    }

    bool contains(const Coords& node) const {
        return find(steps.crbegin(), steps.crend(), node) != steps.crend();
    }

    vector<Path> expand(ElevationGrid grid) const {
        vector<Path> result;

            for (const Direction& dir: { Direction::Up, Direction::Down, Direction::Left, Direction::Right }) {
            if (grid.allowTransition(head(), dir)) {

                const auto newHead = head() + to_Coords(dir);
                if (!contains(newHead)) {
                    Path newPath = *this;
                    newPath.steps.emplace_back(newHead);

                    result.emplace_back(forward<Path>(newPath));
                }
            }
        }
        return result;
    }

    ostream& print(ostream& os, const ElevationGrid& grid, bool dead = false, const unordered_set<Coords>& closed = {}) const {
        // copy target vectors
        vector<vector<char>> result = grid.elevation;

        // clear
        for_each(result.begin(), result.end(), [](auto& row) {
            for_each(row.begin(), row.end(), [](char& val) {
                val = ' ';
            });
        });

        auto put = [&result](const Coords& pos, char ch) {
            result.at(pos.y).at(pos.x) = ch;
        };

        put(grid.goal, 'E');

        for_each(begin(closed), end(closed), [&put](const auto& location) {
            put(location, '.');
        });

        for (auto current = steps.cbegin(); current != steps.cend() - 1; ++current) {
            const auto currentVal = *current;
            auto next = current + 1;
            const auto nextVal = *next;

            const auto d = nextVal - currentVal;

            const auto mapDirToChar = [](const Direction& dir) -> char {
                switch (dir) {
                case Direction::Up: return 'v';
                case Direction::Down: return '^';
                case Direction::Left: return '<';
                case Direction::Right: return '>';

                default:
                    return '*';
                }
            };

            put(currentVal, mapDirToChar(to_Direction(nextVal - currentVal)));
        }

        put(head(), dead ? 'X' : 'H');

        //print
        for_each(result.cbegin(), result.cend(), [](const auto& row) {
            for_each(row.cbegin(), row.cend(), [](const char ch) {
                cout << ch;
            });
        cout << '\n';
        });

        return os;
    }
};

class PathHeuristic {
public:
    using value_type = size_t;

    auto operator() (const Path& path) const {
        return path.length();
    }
};

class PathGoalHeuristic {
public:
    const Coords goal;
    using value_type = double;

    PathGoalHeuristic(const Coords& goal)
        :goal{ goal }
    {

    }

    value_type operator()(const Path& path) const {
        const auto length = static_cast<value_type>(path.length());
        const auto remaining = static_cast<value_type>(manhattan(path.head(), goal));

        return length + remaining;
    }
};

template <typename ValueType, typename KeyRetrieverType, typename HeuristicComputerType, typename HeuristicComparator = less<typename HeuristicComputerType::value_type>>
class updatable_priority_queue {
    struct NodeType {
        HeuristicComputerType::value_type heuristicValue;
        ValueType value;
    };
    vector<NodeType> container;
    const HeuristicComputerType heuristicComputer;
    const HeuristicComparator heuristicComparator;
    const KeyRetrieverType keyRetriever;

public:
    updatable_priority_queue(const KeyRetrieverType& keyRetriever = {}, const HeuristicComputerType& heuristicComputer = {}, const HeuristicComparator& heuristicComparator = {})
        : container{}
        , keyRetriever{ keyRetriever }
        , heuristicComputer{ heuristicComputer }
        , heuristicComparator{ heuristicComparator }
    {
    }

    ValueType popBest() {
        auto best = container.back();
        container.pop_back();

        return best.value;
    }

    void insert(ValueType&& toInsert) {
        const auto newKey = keyRetriever(toInsert);
        const auto newHeuristicValue = heuristicComputer(toInsert);

        // find current element
        auto it = find_if(begin(container), end(container), [&newKey, &capturedKeyRetriever = keyRetriever](const auto& current) {
            return capturedKeyRetriever(current.value) == newKey;
            });

        if (it != container.end()) {
            // found the elemenet with the same key
            if (!heuristicComparator(newHeuristicValue, it->heuristicValue)) {
                // new value doesnt have smaller heuristic than the existing one
                // Keep the existing value and ignore the new one
                return;
            }
            else {
                // new value has smaller heuristic than the existing one. Remove the
                // the existing and proceed to add the new to the appropriate place
                auto new_last = container.erase(it);
            }
        }

        // add new value
        NodeType newNode{ newHeuristicValue, forward<ValueType>(toInsert) };
        auto addLocation = upper_bound(begin(container), end(container), newNode, [&capturedHeuristicComparator = heuristicComparator](const NodeType& lhs, const NodeType& rhs) {
            return !capturedHeuristicComparator(lhs.heuristicValue, rhs.heuristicValue);
            });

        container.insert(addLocation, forward<NodeType>(newNode));
    }

    auto empty() const {
        return container.empty();
    }
};

template <typename TGoalHeuristic, typename TGoalChecker>
class PathResolver {
    const ElevationGrid& grid;

    TGoalHeuristic heuristic;
    TGoalChecker goalChecker;
    updatable_priority_queue<Path, function<Coords(Path)>, TGoalHeuristic> open;
    unordered_set<Coords> closed;

public:
    PathResolver(const decltype(ElevationGrid::start) start, const ElevationGrid& grid, const TGoalHeuristic& heuristic, const TGoalChecker& goalChecker)
        : grid(grid)
        , heuristic{ heuristic }
        , goalChecker{ goalChecker }
        , open{ mem_fn(&Path::head), heuristic }
    {
        open.insert(start);
    }

    optional<Path> findPath() {
        
        while (!open.empty()) {
            const auto current = open.popBest();

            if (goalChecker(current.head())) {
                return optional(current);
            }
            else {
                closed.emplace(current.head());
            }

            auto expanded = current.expand(grid);
            for (auto& next : expanded) {
                if (!closed.contains(next.head())) {
                    open.insert(forward<Path>(next));
                }
            }
        }

        return nullopt;
    }
};

ostream& operator<<(ostream& os, const ElevationGrid& grid) {
    for (const auto& row : grid.elevation) {
        for (const auto& element : row) {
            os << element;
        }
        os << endl;
    }

    return os;
}

DayResult day12() {
    const auto input = readFile("day_12_input.txt");
    ElevationGrid grid(input);
    //cout << grid << endl;


    PathResolver resolver{
        grid.start,
        grid,
        PathGoalHeuristic{grid.goal},
        [goal = grid.goal](const Coords& toCheck) {
            return toCheck == goal;
    } };

    const optional<Path> result1 = resolver.findPath();
    const auto steps = result1.transform(mem_fn(&Path::length)).value_or(0);



    grid.forwardSearch = false;
    PathResolver resolver2{
        grid.goal,
        grid,
        PathHeuristic{},
        [&grid](const Coords& current) {
            return grid.at(current) == 'a';
    } };
    const auto result2 = resolver2.findPath();
    const auto steps2 = result2.transform(mem_fn(&Path::length)).value_or(0);


    //if (result2.has_value()) {
    //    const auto path = result2.value();
    //    cout << "Steps: " << path.length() << endl;
    //    path.print(cout, grid);
    //}

    return {
        make_optional<PartialDayResult>({"Steps", to_string(steps)}),
        make_optional<PartialDayResult>({"Steps", to_string(steps2)})
    };
}
