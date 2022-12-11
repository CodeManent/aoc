#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <numeric>
#include <stdexcept>
#include <map>
#include <span>
#include <ranges>
#include <charconv>
#include <stack>
#include <sstream>
#include <cassert>
#include <optional>
#include <iomanip>

using namespace std;
using namespace std::ranges;

vector<string> readFile(string path) {
    vector<string> data;
    ifstream infile(path);

    string str;
    while (getline(infile, str)) {
        data.push_back(str);
    }

    return data;
    //return { "ab", "cd", "ef" };
}

template<typename T>
ostream& operator<<(ostream& os, const vector<T> data) {
    os << "(";
    size_t index = 0;

    for (auto& i : data) {
        if (index++ != 0) {
            os << ", ";
        }
        os << i;
    }
    os << ")";

    return os;
}

vector<vector<string>> toGroups(vector<string> input) {
    vector<vector<string>> result;

    auto beginIter = input.begin();
    auto currentIter = beginIter;
    while (++currentIter != input.end()) {
        if (currentIter->empty()) {
            // found the empty line;
            result.push_back(vector<string>(beginIter, currentIter));
            beginIter = currentIter + 1;
        }
    }

    return result;
}

void day1()
{
    const auto fileData = readFile("day_1_input.txt");
    // cout << fileData << endl;

    const auto groups = toGroups(fileData);
    // cout << groups << endl;

    vector<vector<int>> intGroups;
    ranges::transform(
        groups,
        back_inserter(intGroups),
        [](const auto& strGroup) {
            vector<int> localResult;
            ranges::transform(
                strGroup,
                back_inserter(localResult),
                [](const auto& str) {return stol(str); });
        
            return localResult;
        });
    // cout << intGroups << endl;

    vector<int> sums;
    ranges::transform(
        intGroups,
        back_inserter(sums),
        [](const auto& group) {
            return accumulate(group.begin(), group.end(), 0);
        }
    );
    // std::cout << sums << endl;

    const auto maxSum = std::ranges::max(sums);
    std::cout << "Max sum: " << maxSum << endl;

    ranges::sort(sums, ranges::greater());
    // std::cout << sums << endl;

    const auto top3Sum = accumulate(sums.begin(), sums.begin() + 3, 0);
    std::cout << "Sum of top 3: " << top3Sum << endl;
}

//========== Day 2

class Day2Play {
    enum class Move {
        Rock,
        Paper,
        Scissors
    };

    enum class Outcome {
        Lose,
        Draw,
        Win
    };

    map<const char, Move> otherMoveMapping {
        {'A', Move::Rock},
        {'B', Move::Paper},
        {'C', Move::Scissors}
    };

    map<const char, Move> myMoveMapping{
        {'X', Move::Rock},
        {'Y', Move::Paper},
        {'Z', Move::Scissors}
    };

    map<const char, Outcome> outcomeMapping{
        {'X', Outcome::Lose},
        {'Y', Outcome::Draw},
        {'Z', Outcome::Win}
    };


    map<Move, Move> winsOver {
        {Move::Rock, Move::Scissors},
        {Move::Paper, Move::Rock},
        {Move::Scissors, Move::Paper}
    };

    map<Move, int> moveScore{
        {Move::Rock, 1},
        {Move::Paper, 2},
        {Move::Scissors, 3}
    };

    map<Outcome, int> outcomeScore{
        {Outcome::Lose, 0},
        {Outcome::Draw, 3},
        {Outcome::Win, 6}
    };

    Move otherPlay;
    Move myPlay;
    Outcome outcome;

    const Outcome computeOutcome() {
        if (myPlay == otherPlay) {
            return Outcome::Draw;
        }

        if (winsOver[myPlay] == otherPlay) {
            return Outcome::Win;
        }

        return Outcome::Lose;
    }

    const Move computeMyPLay() {
        if (outcome == Outcome::Draw) {
            return otherPlay;
        }

        if (outcome == Outcome::Lose) {
            return winsOver[otherPlay];
        }

        // find a losing move
        for (const auto& entry : winsOver) {
            if (entry.second == otherPlay) {
                return entry.first;
            }
        }

        throw exception("Couldn't compute an appropriate move");
    }

public:
    Day2Play(const string& playDef) {
        if (playDef.size() != 3 || playDef[1] != ' ') {
            throw std::exception("Not a valid play");
        }

        otherPlay = otherMoveMapping[playDef[0]];
        myPlay = myMoveMapping[playDef[2]];
        outcome = outcomeMapping[playDef[2]];
    }

    void fixOutcome() {
        outcome = computeOutcome();
    }

    void fixMyPLay() {
        myPlay = computeMyPLay();
    }

    const int score() {

        return moveScore[myPlay] + outcomeScore[outcome];
    }
};

void day2() {
    const auto lines = readFile("day_2_input.txt");

    const int forwardResult = accumulate(lines.begin(), lines.end(), 0, [](int&& currentScore, const string& line) {
        Day2Play play(line);
        play.fixOutcome();
        const int lineScore = play.score();

        return currentScore + lineScore;
    });

    cout << "Forward accumulated score: " << forwardResult << endl;

    const int reverseResult = accumulate(lines.begin(), lines.end(), 0, [](int&& currentScore, const string& line) {
        Day2Play play(line);
        play.fixMyPLay();
        const int lineScore = play.score();

        return currentScore + lineScore;
        }
    );
    cout << "Reverse accumulated score: " << reverseResult << endl;
}


//========== Day 3

class Rucksack {
    string contents;

public:
    Rucksack(const string input):contents(input) {

    }

    char findCommonInCompartments() {
        auto begin = contents.begin();
        auto end = contents.end();
        auto mid = begin + std::distance(begin, end) / 2;

        span<char> compartment1(begin, mid);
        span<char> compartment2(mid, end);

        std::sort(compartment1.begin(), compartment1.end());
        std::sort(compartment2.begin(), compartment2.end());

        auto i1 = compartment1.begin();
        auto i2 = compartment2.begin();

        while (i1 != compartment1.end() && i2 != compartment2.end()) {
            if (*i1 == *i2) {
                return *i1;
            }
            if (*i1 < *i2) {
                ++i1;
            }
            else {
                ++i2;
            }
        }
        throw runtime_error("No common item was found");
    }


};

int itemPriority(const char ch) {
    if ('a' <= ch && ch <= 'z') {
        return ch - 'a' + 1;
    }
    else {
        return ch - 'A' + 27;
    }
}

char findCommon(string str1, string str2, string str3) {
    ranges::sort(str1);
    ranges::sort(str2);
    ranges::sort(str3);

    const auto [first1, last1] = ranges::unique(str1);
    const auto [first2, last2] = ranges::unique(str2);
    const auto [first3, last3] = ranges::unique(str3);

    std::vector<char> intermediate;
    std::set_intersection(str1.begin(), first1, str2.begin(), first2, back_inserter(intermediate));

    std::vector<char> result;
    std::set_intersection(intermediate.cbegin(), intermediate.cend(), str3.begin(), first3, back_inserter(result));

    if (result.size() != 1) {
        throw runtime_error("Bad result");
    }
    return result[0];
}

void day3() {
    const auto lines = readFile("day_3_input.txt");

    const auto prioritySum = std::accumulate(lines.begin(), lines.end(), 0, [](int currentSum, const auto line) {
            Rucksack sack(line);
            return currentSum + itemPriority(sack.findCommonInCompartments());
        }
    );

    cout << "Priority sum: " << prioritySum << endl;

    int badgePrioritySum = 0;
    auto iter = lines.begin();
    while (iter != lines.end()) {
        const auto badge = findCommon(*iter, *(iter + 1), *(iter + 2));
        const auto priority = itemPriority(badge);
        badgePrioritySum += priority;
        iter += 3;
    }

    cout << "Badge priority sum: " << badgePrioritySum << endl;
}

//========== Day 4

class Range {
    int start;
    int end;

public:
    Range(int start, int end)
        :start(start)
        , end(end)
    {
    }

    bool contains(const int point) const {
        return start <= point && point <= end;
    }

    bool contains(const Range& other) const {
        return start <= other.start && other.end <= end;
    }

    bool overlaps(const Range& other) const {
        return contains(other) || other.contains(*this) || contains(other.start) || contains(other.end);
    }

    friend ostream& operator<<(ostream&, const Range&);
};

ostream& operator<<(ostream& os, const Range &r) {
    os << '[' << r.start << '-' << r.end << ']';

    return os;
}

Range parseRange(const auto& input) {
    const auto dashLocation = input.begin() + input.find('-');

    int begin = 0;
    string beginStr{ input.begin(), dashLocation };
    if (const auto [ptr, ec] = from_chars(beginStr.data(), beginStr.data() + beginStr.length(), begin); ec != std::errc()) {
        throw runtime_error("Failed to parse the begin number");
    }

    int end = 0;
    const string strEnd{ dashLocation + 1, input.end() };
    if (const auto [ptr, ec] = from_chars(strEnd.data(), strEnd.data() + strEnd.length(), end); ec != std::errc()) {
        throw runtime_error("Failed to parse the end number");
    }

    return Range{ begin, end };
}

tuple<Range, Range> parseRangesPair(const string line) {
    const auto commaLocation = line.cbegin() + line.find(',');
    string_view range1Str(line.cbegin(), commaLocation);
    string_view range2Str(commaLocation + 1, line.cend());

    auto range1 = parseRange(range1Str);
    auto range2 = parseRange(range2Str);

    return { range1, range2 };
}

void day4() {
    const auto lines = readFile("day_4_input.txt");
    cout << "Lines: " << lines.size() << endl;

    int fullyContainedSum = 0;
    int overlapsSum = 0;
    for (const auto& line : lines) {
        const auto [range1, range2] = parseRangesPair(line);

        bool fullyContained = range1.contains(range2) || range2.contains(range1);
        if (fullyContained) {
            ++fullyContainedSum;
        }

        if (range1.overlaps(range2)) {
            ++overlapsSum;
        }
    }
    cout << "Fully contined ranges count: " << fullyContainedSum << endl;
    cout << "Overlapping ranges: " << overlapsSum << endl;

}

//========== Day 5
class TaggedStack {
public:
    const string tag;
    std::stack<char> stack;

    TaggedStack(const string& tag)
        :tag(tag)
    {
    }
    
    friend ostream& operator<< (ostream&, const TaggedStack&);
};

template<typename T>
ostream& operator<<(ostream& os, const ranges::subrange<T>& r) {
    os << '(';
    if (!r.empty()) {
        auto it = r.begin();
        os << *it;
        while (++it != r.end()) {

            os << ", " << *it;
        }
    }
    os << ')';

    return os;

}

template <typename T>
ostream& operator<<(ostream& os, const stack<T> q) {
    const auto& c = q._Get_container();
    return os << ranges::subrange{ c.begin(), c.end() };
}

ostream& operator<< (ostream& os, const TaggedStack& tq) {
    return os << "{[" << tq.tag << "]: " << tq.stack << "}";
}

vector<TaggedStack> parseStacks(const auto &strLabels, const auto& lines) {

    // create the stacks based on the labels line
    istringstream iss(strLabels);
    auto v = subrange{ istream_iterator<string>(iss), istream_iterator<string>() }
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

class StackCommand {
public:
    string source;
    string target;
    int amount;

    StackCommand(string source, string target, int amount)
        :source(source)
        , target(target)
        , amount(amount)
    {
    }
};



class ParsingError : public runtime_error {
public:
    ParsingError(const string& reason)
    :runtime_error(reason)
    {
    }
};
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
    
    return {from, to, amount};
}

string getStacksMessage(const vector<TaggedStack>& stacks) {
    stringstream ss;
    ranges::for_each(stacks, [&ss](const auto& st) {
        const auto value = st.stack.top();
        ss << value;
    });
    return ss.str();
}

void day5() {
    const auto lines = readFile("day_5_input.txt");

    // find empty lines (stacks-commands separator)
    const auto emptyLine = std::find_if(lines.begin(), lines.end(), [](const auto& str) {return str.empty(); });

    const auto& labelsLine = *(emptyLine - 1);
    const auto stacksDef = ranges::subrange(lines.begin(), emptyLine-1);
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
                //throw runtime_error("Not enough in the source while executing command");
                cerr << "Not enough in the source while executing command: " << line << endl;
                continue;
            }
            commandProcessor(source, destination, cmd.amount);
        }

        std::cout << "Crates message: " << getStacksMessage(stacks) << endl;
    };

    stacksProcessor([](auto& src, auto& dst, int amount) {
        for (int i = 0; i < amount; ++i) {
            const char value = src.top();
            dst.push(value);
            src.pop();
        }
    });


    stacksProcessor([](auto& src, auto& dst, int amount) {
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

}


//========== Day 6

size_t find_unique(const string& input, const size_t window) {
    // check if window is ok. if not, push it after the last matching pair

    for (size_t w = window - 1; w < input.size(); ) {
        for (size_t x = 0; x < window-1; ++x) {
            const char wChar = input[w - x];
            for (size_t y = x + 1; y < window; ++y) {
                const char xChar = input[w - y];
                if (wChar == xChar) {
                    // collision character was found. Bump the window accordingly to exclude the 'y' character
                    w += window - y;
                    goto check_next_window;
                }
            }
        }
        // no collision was found
        return w + 1;

    check_next_window:
        ;

    }
    throw ParsingError("Exceeded input");
}


size_t startOfPacket(const string& input) {
    if (input.size() < 4) {
        throw runtime_error("Not enough symbolds to detect the start_of_packet");
    }

    return find_unique(input, 4);
}

size_t startOfMessage(const string& input) {
    if (input.size() < 14) {
        throw runtime_error("Not enough symbolds to detect the start_of_message");
    }
    return find_unique(input, 14);
}

void day6() {
    assert(startOfPacket("bvwbjplbgvbhsrlpgdmjqwftvncz") == 5);
    assert(startOfPacket("nppdvjthqldpwncqszvftbrmjlhg") == 6);
    assert(startOfPacket("nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg") == 10);
    assert(startOfPacket("zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw") == 11);
    cout << "Assertions ok" << endl;

    ifstream infile("day_6_input.txt");
    std::string input{ istream_iterator<char>(infile), istream_iterator<char>() };

    cout << "Start of packet: " << startOfPacket(input) << endl;

    assert(startOfMessage("mjqjpqmgbljsphdztnvjfqwrcgsmlb") == 19);
    assert(startOfMessage("bvwbjplbgvbhsrlpgdmjqwftvncz") == 23);
    assert(startOfMessage("nppdvjthqldpwncqszvftbrmjlhg") == 23);
    assert(startOfMessage("nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg") == 29);
    assert(startOfMessage("zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw") == 26);

    cout << "Start of message: " << startOfMessage(input) << endl;
}



//========== Day 7

class Filesystem {
public:
    //forward declare classes
    class File;
    class Directory;

    class Visitor {
    public:
        virtual void visit(File&) = 0;
        virtual void visit(Directory&) = 0;
    };

    class Entry {
    public:
        string name;
        Entry(string name)
            :name(name)
        {
        }
        virtual ~Entry() = default;
        virtual void accept(Visitor&) = 0;
    };

    class File : public Entry {
    public:
        size_t size;

        File(string name, size_t size)
            :Entry(name)
            , size(size)
        {}

        virtual ~File() = default;
        virtual void accept(Visitor& v) {
            v.visit(*this);
        }
    };

    class Directory : public Entry {
    public:
        vector<unique_ptr<Entry>> contents;
        Directory(string name)
            :Entry(name)
            , contents{}
        {
        }
        Directory(Directory&&) = default;
        virtual ~Directory() = default;
        virtual void accept(Visitor& v) {
            v.visit(*this);
        }
    };

    stack<Directory*> dirstack;
    Directory root;

public:
    Filesystem()
        :dirstack{}
        , root(Directory{ "/" })
    {
        // Add the root directory
        cd("/");
    }
    
    Filesystem(Filesystem&& other) = default;

    Directory& cd(string name) {
        if (name == "..") {
            dirstack.pop();
        }
        else if (name == "/") {
            while (!dirstack.empty()) {
                dirstack.pop();
            }
            dirstack.push(&root);
        }
        else {
            auto subdirIter = ranges::find_if(dirstack.top()->contents, [&name](const auto& d) { return d->name == name; });
            auto subdirPrt = static_cast<Directory*>(subdirIter->get());
            dirstack.push(subdirPrt);
        }

        return *dirstack.top();
    }

    void addFile(string name, size_t size) {
        dirstack.top()->contents.push_back(make_unique<File>(name, size));
    }

    void addDir(string name) {
        dirstack.top()->contents.push_back(make_unique<Directory>(name));
    }

    void accept(Visitor& v) {
        dirstack.top()->accept(v);
    }
};

Filesystem parseConsoleInput(const vector<string>& input) {
    Filesystem fs;

    for (auto line = input.begin(); line != input.end(); ++line) {
        istringstream iss(*line);
        string token;
        iss >> token;
        if (token != "$") {
            throw ParsingError("Not a command");
        }

        iss >> token;
        if (token == "cd") {
            string dirToken;
            iss >> dirToken;
            fs.cd(dirToken);
        }
        if (token == "ls") {
            // parse response lines until a command lins is found
            while ((line + 1) != input.end() && (line + 1)->at(0) != '$') {
                ++line;

                // parse ls response line
                string lsToken1, lsToken2;
                istringstream iss2(*line);
                iss2 >> lsToken1 >> lsToken2;
                if (lsToken1 == "dir") {
                    fs.addDir(lsToken2);
                }
                else {
                    const size_t size = atoi(lsToken1.data());
                    fs.addFile(lsToken2, size);
                }
            }
        }


    }


    return fs;
}

class FSPrinter : public Filesystem::Visitor {
    size_t depth = 0;
public:
    FSPrinter(size_t depth)
        :depth(depth)
    {
    }

    FSPrinter()
    :FSPrinter(0)
    {

    }

    virtual void visit(Filesystem::File& f) {
        cout << string(depth*2, ' ') << "- " << f.name << " (file, size=" << f.size << ")" << endl;
    }

    virtual void visit(Filesystem::Directory& d) {
        cout << string(depth*2, ' ') << "- " << d.name << " (dir)" << endl;
        ++depth;
        ranges::for_each(d.contents, [this](const auto& d) { d->accept(*this);});
        --depth;
    }
};

class SizeComputer : public Filesystem::Visitor {
public:
    class SizeReporter {
    public:
        virtual void report(Filesystem::File&, size_t) {
            //noop by default
        }
        virtual void report(Filesystem::Directory&, size_t) {
            //noop by default
        }
    };

    SizeReporter& reporter;
    size_t dirSize = 0;

    SizeComputer(SizeReporter& r)
        :reporter(r)
    {
    }

    virtual void visit(Filesystem::File& f) {
        dirSize += f.size;
        reporter.report(f, f.size);
    }

    virtual void visit(Filesystem::Directory& d) {
        for (const auto& entry : d.contents) {
            SizeComputer subdirVisitor(reporter);

            entry->accept(subdirVisitor);

            dirSize += subdirVisitor.dirSize;
        }

        reporter.report(d, dirSize);
    }
};

class FilteredDirSizeReporter : public SizeComputer::SizeReporter {
public:
    size_t accumulatedSize = 0;

    virtual void report(Filesystem::Directory& d, size_t size) {
        // cout << "Visited directory: " << d.name << " of size " << accumulatedSize << endl;
        if (size <= 100000) {
            //cout << "Addig " << d.name << " (size: " << size << ")" << endl;
            accumulatedSize += size;
        }
    }
};

class DeleteTargetSelector : public SizeComputer::SizeReporter {
public:
    static const size_t totalSpace = 70000000;
    static const size_t neededSpace = 30000000;

    const size_t usedSpace;
    const size_t needToFree;

    struct SelectedDirectory {
        string name;
        size_t size;
    };
    optional<SelectedDirectory> selected;

    DeleteTargetSelector(const size_t usedSpace)
        :usedSpace(usedSpace)
        ,needToFree(neededSpace - (totalSpace - usedSpace))
        , selected{}
    {
        assert(needToFree > 0);
    }

    virtual void report(Filesystem::Directory& d, size_t size) {
        if (size >= needToFree && (!selected.has_value() || size < selected->size)) {
            selected = make_optional<SelectedDirectory>({ d.name, size });
        }
    }
};

void day7() {
    const auto consoleLog = readFile("day_7_input.txt");
    auto fs = parseConsoleInput(consoleLog);

    fs.cd("/");

    //FSPrinter printer;
    //fs.accept(printer);

    FilteredDirSizeReporter reporter;
    SizeComputer visitor(reporter);

    fs.accept(visitor);

    cout << "Root dir size: " << visitor.dirSize << endl;
    cout << "Reported (filtered) accumulated size " << reporter.accumulatedSize << endl;

    DeleteTargetSelector selector(visitor.dirSize);
    SizeComputer visitor2(selector);

    fs.accept(visitor2);

    if (selector.selected.has_value()) {
        cout << "Selected dir for deletion: " << selector.selected->name << " of size " << selector.selected->size << endl;
    }
    else {
        cerr << "No dir was selected for deletion" << endl;
    }
}

//========== Day 8

class Point {
public:
    int x;
    int y;

    Point()
        : Point(0, 0)
    {}

    Point(int x, int y)
        : x(x)
        , y(y)
    {
    }
};

class Grid {
public:
    class Location {
    public:
        short treeSize;
        short maxUp;
        short maxDown;
        short maxLeft;
        short maxRight;
        bool isVisible;
        short viewDistanceUp;
        short viewDistanceDown;
        short viewDistanceLeft;
        short viewDistanceRight;
        long scenicScore;

        Location(short treeSize)
            :treeSize(treeSize)
            , maxUp(0)
            , maxDown(0)
            , maxLeft(0)
            , maxRight(0)
            , isVisible(true)
            , viewDistanceUp(0)
            , viewDistanceDown(0)
            , viewDistanceLeft(0)
            , viewDistanceRight(0)
            , scenicScore(0)
        {
        }
    };

    vector<Location> backingStore;
    size_t n;

    Grid(size_t n, auto locationGemerator)
        :n(n)
        , backingStore{}
    {
        backingStore.reserve(n * n);

        for (size_t y = 0; y < n; ++y) {
            for (size_t x = 0; x < n; ++x) {
                backingStore.emplace_back(locationGemerator(x, y));
            }
        }
    }


    const Location& at(const size_t x, const size_t y) const{
        assert(x >= 0);
        assert(y >= 0);
        assert(x <= n);
        assert(y <= n);

        return backingStore.at(y * n + x);
    }

    Location& at(const size_t x, const size_t y) {
        assert(x >= 0);
        assert(y >= 0);
        assert(x <= n);
        assert(y <= n);

        return backingStore.at(y * n + x);
    }

    void computeVisibility() {
        using std::max;

        // top->bottom, left->right

        visitTopDownLeftRight([this](const size_t x, const size_t y, Location& current) {
            if (x > 0) {
                const auto& left = at(x - 1, y);
                current.maxLeft = max(left.maxLeft, left.treeSize);

                current.viewDistanceLeft = 1;
            loop_compute_view_distance_left:
                size_t viewX = x - current.viewDistanceLeft;
                if (viewX > 0) {
                    const auto& viewTarget = at(viewX, y);
                    if (current.treeSize > viewTarget.treeSize && viewTarget.viewDistanceLeft > 0) {
                        current.viewDistanceLeft += viewTarget.viewDistanceLeft;
                        goto loop_compute_view_distance_left;
                    }
                }
            }

            if (y > 0) {
                const auto& up = at(x, y - 1);
                current.maxUp = max(up.maxUp, up.treeSize);

                current.viewDistanceUp = 1;
            loop_compute_view_distance_up:
                size_t viewY = y - current.viewDistanceUp;
                if (viewY > 0) {
                    const auto& viewTarget = at(x, viewY);
                    if (current.treeSize > viewTarget.treeSize && viewTarget.viewDistanceUp > 0) {
                        current.viewDistanceUp += viewTarget.viewDistanceUp;
                        goto loop_compute_view_distance_up;
                    }
                }
            }
        });

        visitBottomUpRightLeft([this](const size_t x, const size_t y, Location& current) {
            if (x + 1 != n) {
                const auto& right = at(x + 1, y);
                current.maxRight = max(right.maxRight, right.treeSize);

                current.viewDistanceRight = 1;
            loop_compute_view_distance_right:
                size_t viewX = x + current.viewDistanceRight;
                if (viewX < n - 1) {
                    const auto& viewTarget = at(viewX, y);
                    if (current.treeSize > viewTarget.treeSize && viewTarget.viewDistanceRight > 0) {
                        current.viewDistanceRight += viewTarget.viewDistanceRight;
                        goto loop_compute_view_distance_right;
                    }
                }
            }

            if (y + 1 != n) {
                const auto& down = at(x, y + 1);
                current.maxDown = max(down.maxDown, down.treeSize);

                current.viewDistanceDown = 1;
            loop_compute_view_distance_down:
                size_t viewY = y + current.viewDistanceDown;
                if (viewY < n - 1) {
                    const auto& viewTarget = at(x, viewY);
                    if (current.treeSize > viewTarget.treeSize && viewTarget.viewDistanceDown > 0) {
                        current.viewDistanceDown += viewTarget.viewDistanceDown;
                        goto loop_compute_view_distance_down;
                    }
                }
            }
        });

        visitTopDownLeftRight([this](const size_t x, const size_t y, Location& current) {
            current.isVisible = x == 0
                || x == n - 1
                || y == 0
                || y == n - 1
                || current.maxDown < current.treeSize
                || current.maxUp < current.treeSize
                || current.maxLeft < current.treeSize
                || current.maxRight < current.treeSize;

            current.scenicScore = current.viewDistanceLeft
                * current.viewDistanceRight
                * current.viewDistanceUp
                * current.viewDistanceDown;
        });
    }

    void visitTopDownLeftRight(auto callback) {
        for (size_t y = 0; y < n; ++y) {
            for (size_t x = 0; x < n; ++x) {
                callback(x, y, at(x, y));
            }
        }
    }

    void visitTopDownLeftRight(auto callback) const {
        for (size_t y = 0; y < n; ++y) {
            for (size_t x = 0; x < n; ++x) {
                callback(x, y, at(x, y));
            }
        }
    }

    void visitBottomUpRightLeft(auto callback) {
        for (size_t shiftedY = n; shiftedY > 0; --shiftedY) {
            for (size_t shiftedX = n; shiftedX > 0; --shiftedX) {
                // Use shifted[X/Y] because we can't use the actual x/y on the for
                const size_t x = shiftedX - 1;
                const size_t y = shiftedY - 1;

                callback(x, y, at(x, y));
            }
        }
    }

    void visitBottomUpRightLeft(auto callback) const {
        for (size_t shiftedY = n; shiftedY > 0; --shiftedY) {
            for (size_t shiftedX = n; shiftedX > 0; --shiftedX) {
                // Use shifted[X/Y] because we can't use the actual x/y on the for
                const size_t x = shiftedX - 1;
                const size_t y = shiftedY - 1;

                callback(x, y, at(x, y));
            }
        }
    }

    friend ostream& operator<<(ostream& os, const Grid& g);
};

ostream& operator<<(ostream& os, const Grid& g) {
    g.visitTopDownLeftRight([&g](size_t x, size_t y, const Grid::Location& current) {
        cout << setw(2) <<  current.treeSize;

        if (x + 1 == g.n) {
            cout << endl;
        }
    });

    cout << endl;

    g.visitTopDownLeftRight([&g](size_t x, size_t y, const Grid::Location& current) {
        cout << ' ' << (current.isVisible ? "." : " ");

        if (x + 1 == g.n) {
            cout << endl;
        }
    });
    cout << endl;

    std::cout << "ViewDistanceLeft" << endl;
    g.visitTopDownLeftRight([&g](size_t x, size_t y, const Grid::Location& current) {
        cout << setw(3) << current.viewDistanceLeft;

        if (x + 1 == g.n) {
            cout << endl;
        }
    });

    std::cout << "ViewDistanceRight" << endl;
    g.visitTopDownLeftRight([&g](size_t x, size_t y, const Grid::Location& current) {
        cout << setw(3) << current.viewDistanceRight;

        if (x + 1 == g.n) {
            cout << endl;
        }
    });

    std::cout << "ViewDistanceUp" << endl;
    g.visitTopDownLeftRight([&g](size_t x, size_t y, const Grid::Location& current) {
        cout << setw(3) << current.viewDistanceUp;

        if (x + 1 == g.n) {
            cout << endl;
        }
    });

    std::cout << "viewDistanceDown" << endl;
    g.visitTopDownLeftRight([&g](size_t x, size_t y, const Grid::Location& current) {
        cout << setw(3) << current.viewDistanceDown;

        if (x + 1 == g.n) {
            cout << endl;
        }
    });

    std::cout << "Scenic score" << endl;
    g.visitTopDownLeftRight([&g](size_t x, size_t y, const Grid::Location& current) {
        cout << setw(3) << current.scenicScore;

        if (x + 1 == g.n) {
            cout << endl;
        }
    });

    return os;
}


void day8() {
    const auto input = readFile("day_8_input.txt");

    const size_t n = std::max(input[0].length(), input.size());
    const auto inputLocationMapper = [&input](const size_t x, const size_t y) -> int {
        return input[y][x] - '0';
    };

    Grid grid(n, inputLocationMapper);
    grid.computeVisibility();
    // cout << "Grid:\n" << grid << endl;

    size_t visibleCount = 0;
    long maxScenicScore = 0;
    grid.visitTopDownLeftRight([&visibleCount, &maxScenicScore](auto, auto, const Grid::Location& current) {
        if (current.isVisible) {
            ++visibleCount;
        }
        maxScenicScore = std::max(maxScenicScore, current.scenicScore);
    });
    cout << "Visible trees: " << visibleCount << endl;

    cout << "Max scenic score: " << maxScenicScore << endl;
}

/*
* 
* 
* 
* 
* 
                                 MAIN                             
*
*
*
*
*
*/

int main() {
    cout << "Day 1" << endl;
    day1();

    cout << endl << "Day 2" << endl;
    day2();

    cout << endl << "Day 3" << endl;
    day3();

    cout << endl << "Day 4" << endl;
    day4();

    cout << endl << "Day 5" << endl;
    day5();

    cout << endl << "Day 6" << endl;
    day6();

    cout << endl << "Day 7" << endl;
    day7();

    cout << endl << "Day 8" << endl;
    day8();

    return 0;
}
