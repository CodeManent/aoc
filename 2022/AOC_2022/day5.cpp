#include "day.h"
#include "utils.h"
#include "printers.h"

#include "TaggedStack.h"
#include "StackCommand.h"
#include "ParsingError.h"

#include <sstream>
#include <ranges>
#include <algorithm>
#include <iterator>

using namespace std;

vector<TaggedStack> parseStacks(const auto& strLabels, const auto& lines) {

    // create the stacks based on the labels line
    istringstream iss(strLabels);
    auto v = ranges::subrange{ istream_iterator<string>(iss), istream_iterator<string>() }
        | views::transform([](const auto& value) {
        return TaggedStack(value);
            });

    vector<TaggedStack> stacks;
    ranges::copy(v, back_inserter(stacks));

    // fill the stacks
    for (const auto line : views::reverse(lines)) {
        for (int stackIndex = 0; 4 * stackIndex + 2 < line.length(); ++stackIndex) {
            const int i = 4 * stackIndex;

            if (line[i] == '[' && line[i + 2] == ']') {
                stacks[stackIndex].stack.push(line[i + 1]);
            }
        }
    }

    return stacks;
}


StackCommand parseStackCommand(const string& input) {
    istringstream iss(input);
    int amount;
    string from, to;

    string dummy;
    iss >> dummy;
    if (dummy != "move") {
        throw ParsingError("\"move\" not detected");
    }
    iss >> amount;

    iss >> dummy;
    if (dummy != "from") {
        throw ParsingError("\"from\" not detected");
    }
    iss >> from;

    iss >> dummy;
    if (dummy != "to") {
        throw ParsingError("\"frtoom\" not detected");
    }
    iss >> to;

    return { from, to, amount };
}

string getStacksMessage(const vector<TaggedStack>& stacks) {
    stringstream ss;
    ranges::for_each(stacks, [&ss](const auto& st) {
        const auto value = st.stack.top();
    ss << value;
        });
    return ss.str();
}

DayResult day5() {
    const auto lines = readFile("day_5_input.txt");

    // find empty lines (stacks-commands separator)
    const auto emptyLine = std::find_if(lines.begin(), lines.end(), [](const auto& str) {return str.empty(); });

    const auto& labelsLine = *(emptyLine - 1);
    const auto stacksDef = ranges::subrange(lines.begin(), emptyLine - 1);
    const auto commandsDef = ranges::subrange(emptyLine + 1, lines.end());

    /* part 1 */
    auto stacksProcessor = [labelsLine, stacksDef, commandsDef](auto commandProcessor)
    {
        auto stacks = parseStacks(labelsLine, stacksDef);


        for (const auto& line : commandsDef) {
            const auto cmd = parseStackCommand(line);

            const auto findSourceByTag = [&cmd](const auto& st) { return st.tag == cmd.source; };
            const auto findTargetByTag = [&cmd](const auto& st) { return st.tag == cmd.target; };

            auto& source = ranges::find_if(stacks, findSourceByTag)->stack;
            auto& destination = ranges::find_if(stacks, findTargetByTag)->stack;


            if (source.size() < cmd.amount) {
                throw runtime_error("Not enough in the source while executing command");
                continue;
            }
            commandProcessor(source, destination, cmd.amount);
        }

        //std::cout << "Crates message: " << getStacksMessage(stacks) << endl;
        return getStacksMessage(stacks);
    };

    string msg1 = stacksProcessor([](auto& src, auto& dst, int amount) {
        for (int i = 0; i < amount; ++i) {
            const char value = src.top();
            dst.push(value);
            src.pop();
        }
        });


    string msg2 = stacksProcessor([](auto& src, auto& dst, int amount) {
        stack<char> tmp;
        for (int i = 0; i < amount; ++i) {
            tmp.push(src.top());
            src.pop();
        }
        for (int i = 0; i < amount; ++i) {
            dst.push(tmp.top());
            tmp.pop();
        }
    });

    return {
        make_optional<PartialDayResult>({"Crates message", msg1}),
        make_optional<PartialDayResult>({"Crates message", msg2})
    };
}