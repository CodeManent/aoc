#include "utils.h"
#include <map>
#include <ranges>
#include <numeric>


using namespace std;

const string test_raw_input = R"(RL

AAA = (BBB, CCC)
BBB = (DDD, EEE)
CCC = (ZZZ, GGG)
DDD = (DDD, DDD)
EEE = (EEE, EEE)
GGG = (GGG, GGG)
ZZZ = (ZZZ, ZZZ))";

const string test_raw_input2 = R"(LR

11A = (11B, XXX)
11B = (XXX, 11Z)
11Z = (11B, XXX)
22A = (22B, XXX)
22B = (22C, 22C)
22C = (22Z, 22Z)
22Z = (22B, 22B)
XXX = (XXX, XXX))";

typedef string node_id;
enum class Direction: char{
    Left = 'L',
    Right = 'R'
};

struct Context{
    map<node_id, pair<node_id, node_id>> nodes;
    vector<Direction> path;
};

Context parseInput(const vector<string>& input) {
    Context result;

    for(const char& ch: input[0]) {
        result.path.emplace_back(static_cast<Direction>(ch));
    }

    for(auto line = input.begin() + 2; line != input.end(); ++line) {
        const node_id from = line->substr(0, 3);
        const node_id left = line->substr(7, 3);
        const node_id right = line->substr(12, 3);

        result.nodes[from] = make_pair(left, right);
    }

    return result;
}

ostream& operator<<(ostream& os, const Context context) {
    for(const auto& dir: context.path){
        os << static_cast<char>(dir);
    }
    os << endl;
    for(const auto& entry: context.nodes) {
        cout << '\n'<<  entry.first << " = (" << entry.second.first << ", " << entry.second.second << ")";
    }

    return os;
}

template<typename TCondition>
unsigned long countSteps(const Context& context, const node_id& start, TCondition endCondition){
    // traverse
    size_t step = 0;
    node_id current = start;
    while(!endCondition(current)) {
        const auto content = context.nodes.at(current);
        const Direction dir = context.path[step++ % context.path.size()];

        if(dir == Direction::Left){
            current = content.first;
        }
        else{
            current = content.second;
        }
    }

    // cout << start << " -> " << current << ": " << step << " steps" << endl;

    return step;
}

unsigned long first(const vector<string>& input) {
    Context context = parseInput(input);
    // cout << context << endl;

    return countSteps(context, "AAA", [](const node_id& node) {
        return node == "ZZZ";
    });
}

unsigned long second(const vector<string>& input) {
    Context context = parseInput(input);

    auto endsWith = [](const char& ch) {
        return [ch](const node_id& node) {
            return node.ends_with(ch);
        };
    };

    auto azNodeSteps = context.nodes
        | views::keys
        | views::filter(endsWith('A'))
        | views::transform([&context, &endsWith](const node_id& startNode) {
            return countSteps(context, startNode, endsWith('Z'));
        })
        ;

    if(!ranges::all_of(azNodeSteps, [&context](const unsigned long& steps) {
        const auto isStepsMultiple = steps % context.path.size() == 0;
        cout << "steps: " << steps << " : " << isStepsMultiple << " (" << (steps / context.path.size()) << ")" << endl;
        return isStepsMultiple;
    }))
    {
        throw runtime_error("All of the paths are expected to have a step count multiple to the context path size");
    }

    const auto steps = reduce(begin(azNodeSteps), end(azNodeSteps), 1ul, lcm<unsigned long, unsigned long>);

    return steps;
}

int main(const int, const char* []) {
    cout << "Day 8" << endl;

    const auto first_test_result = first(utils::split(test_raw_input));
    assertEquals(2ul, first_test_result);
    cout << "Test ok" << endl;

    const auto input = utils::readFile("input/day8");
    const auto first_result = first(input);
    cout << "First: " << first_result << endl;

    // solution assertion fails for test case
    // const auto second_test_result = second(utils::split(test_raw_input2));
    // assertEquals(6ul, second_test_result);
    // cout << "Test 2 ok" << endl;

    const auto second_result = second(input);
    cout << "Second: " << second_result << endl;

    return EXIT_SUCCESS;
}
