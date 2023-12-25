#include "utils.h"
#include <numeric>
#include <ranges>
#include <sstream>
#include <charconv> // from_chars
#include <map>
#include <algorithm> // mismatch

using namespace std;

const string test_raw_input = R"(???.### 1,1,3
.??..??...?##. 1,1,3
?#?#?#?#?#?#?#? 1,3,1,6
????.#...#... 4,1,1
????.######..#####. 1,6,5
?###???????? 3,2,1)";

const string test_line = "?###???????? 3,2,1"s;

struct Row{
    const string board;
    vector<unsigned long> numbers;
};

/**
 * should fit
 * all before should be . or ?
 * all during should be # or ?
 * following should be . or ?
*/
bool is_valid_position(const string_view input, unsigned long springs, size_t pos) {
    if(input.size() < springs + pos){
        return false;
    }
    auto iter = begin(input);
    // "consume" . or ?
    while(pos --> 0){
        const auto ch = *iter++;
        if(ch != '.' && ch != '?') {
            return false;
        }
    }
    // "consume" # or ?
    while(springs --> 0 ) {
        const auto ch = *iter++;
        if(ch != '#' && ch != '?') {
            return false;
        }
    }

    // "consume' trailing . or ?
    if(iter != end(input)) {
        const auto ch = *iter++;
        if(ch != '.' && ch != '?') {
            return false;
        }
    }

    return true;
}

vector<unsigned long> lay_first(const string_view input, size_t springs) {
    vector<unsigned long> possiblePositions;

    for(size_t pos = 0; pos + springs <= input.length(); ++pos) {
        if(is_valid_position(input, springs, pos)){
            possiblePositions.push_back(pos);
        }
    }

    return possiblePositions;
}

struct MemoryKey{
    string board;
    vector<size_t> numbers;

    bool operator<(const MemoryKey& other) const {
        if(const auto strCompare = board.compare(other.board); strCompare != 0) {
            return strCompare < 0;
        }


        if(numbers.size() != other.numbers.size()) {
            return numbers.size() < other.numbers.size();
        }

        const auto [leftPos, rightPos] = mismatch(cbegin(numbers), cend(numbers), cbegin(other.numbers), cend(other.numbers));
        if(leftPos != cend(numbers)){
            return *leftPos < *rightPos;
        }

        return false;
    }
};

map<MemoryKey, size_t> cache;


unsigned long count_arrangements(const string_view input, const vector<size_t>& springSizes) {
    const MemoryKey cacheKey {string{input}, springSizes};
    if(auto found = cache.find(cacheKey); found != cache.end()) {
        return found->second;
    }

    if(springSizes.size() == 0){
        // guard/end condition
        if(!all_of(begin(input), end(input), [](const char& ch) {
            return ch == '.' || ch == '?';
        })) {
            // cout << "arrangements(" << input << ", " << springSizes << ") = " << 0 << endl;
            cache[cacheKey] = 0;
            return 0;
        }
        // cout << "arrangements(" << input << ", " << springSizes << ") = " << 1 << endl;
        cache[cacheKey] = 1;
        return 1;
    }

    const auto currentSprings = springSizes.at(0);

    const auto firstPositions = lay_first(input, currentSprings);
    if(firstPositions.empty()){
        // cout << "arrangements(" << input << ", " << springSizes << ") = " << 0 << endl;
        cache[cacheKey] = 0;
        return 0;
    }

    const vector<size_t> remainingSpringSizes{begin(springSizes) + 1, end(springSizes)};

    unsigned long arrangements = 0;
    for(const auto& firstPos: firstPositions) {
        bool consumeSeparator = (firstPos + currentSprings) != input.size();
        auto subArrangements = count_arrangements(
            input.substr(firstPos + currentSprings + consumeSeparator),
            remainingSpringSizes);

        arrangements += subArrangements;
    }

    // cout << "arrangements(" << input << ", " << springSizes << ") = " << arrangements << endl;
    cache[cacheKey] = arrangements;
    return arrangements;
}

vector<size_t> parse_numbers(const string_view input) {
    vector<size_t> result;
    size_t token_start = 0;
    do{
        auto token_end = input.find_first_of(',', token_start);
        if(token_end == std::string::npos){
            token_end = input.size();
        }

        // ltrim
        while(token_start <= token_end - 1 && !isdigit(input.at(token_start))) {
            ++token_start;
        }
        //rtrim
        while(token_start <= token_end-1 && !isdigit(input.at(token_end - 1))){
            --token_end;
        }

        const string strToken {begin(input) + token_start, begin(input) + token_end};
        size_t num{};
        if(const auto res = from_chars(strToken.data(), strToken.data() + strToken.size(), num); res.ec != std::errc{}){
            throw runtime_error("Couldn't parse number from token: \"" + strToken + "\" full input: \"" + string{input} + "\"");
        }

        result.push_back(num);

        token_start = token_end + 1; // skip separator
    }while(token_start < input.size());

    // cout << "Parsed numbers: " << result << endl;
    return result;
}

unsigned long count_arrangements(const string_view& input) {
    const auto splitPos = input.find_first_of(' ');
    const auto board = input.substr(0, splitPos);
    const auto numbers = parse_numbers(input.substr(splitPos + 1));

    return count_arrangements(board, numbers);
}

unsigned long count_expanded_arrangements(const string_view& input) {
    const auto splitPos = input.find_first_of(' ');
    const auto board = input.substr(0, splitPos);
    const auto numbers = parse_numbers(input.substr(splitPos + 1));

    ostringstream oss;
    oss << board;
    vector<unsigned long> expandedNumbers = numbers;
    for(int i = 0; i < 4; ++i) {
        oss << '?' << board;
        expandedNumbers.insert(expandedNumbers.end(), numbers.begin(), numbers.end());
    }
    const auto expandedBoard = oss.str();

    // cout << "count_arrangements(\"" << expandedBoard << "\", " << expandedNumbers << ")" << endl;
    return count_arrangements(expandedBoard, expandedNumbers);
}


unsigned long first(const vector<string>& input) {
    const auto arrangements = transform_reduce(
        begin(input),
        end(input),
        0ul,
        plus<size_t>(),
        [](const auto& str) {
            return count_arrangements(str);
        });

    return arrangements;
}

unsigned long second(const vector<string>& input) {
    const auto arrangements = transform_reduce(
        begin(input),
        end(input),
        0ul,
        plus<size_t>(),
        [](const auto& str) {
            const auto& res = count_expanded_arrangements(str);
            // cout << "count_expanded_arrangements(" << str << ") = " << res << endl;
            return res;
        });

    return arrangements;
}

int main(const int, const char* []) {
    cout << "Day Template" << endl;

    assertEquals(1ul, count_arrangements("? 1"));

    assertEquals(10ul, count_arrangements(test_line));

    assertEquals(1ul, count_arrangements("???.### 1,1,3"));
    assertEquals(4ul, count_arrangements(".??..??...?##. 1,1,3"));
    assertEquals(1ul, count_arrangements("?#?#?#?#?#?#?#? 1,3,1,6"));
    assertEquals(1ul, count_arrangements("????.#...#... 4,1,1"));
    assertEquals(4ul, count_arrangements("????.######..#####. 1,6,5"));
    assertEquals(10ul, count_arrangements("?###???????? 3,2,1"));

    const auto first_test_result = first(utils::split(test_raw_input));
    assertEquals(21ul, first_test_result);
    cout << "Test ok" << endl;

    const auto input = utils::readFile("input/day12");
    const auto first_result = first(input);
    cout << "First: " << first_result << endl;

    assertEquals(1ul, count_arrangements("???.###????.###????.###????.###????.### 1,1,3,1,1,3,1,1,3,1,1,3,1,1,3"));

    assertEquals(1ul, count_expanded_arrangements("???.### 1,1,3"));
    assertEquals(16384ul, count_expanded_arrangements(".??..??...?##. 1,1,3"));
    assertEquals(1ul, count_expanded_arrangements("?#?#?#?#?#?#?#? 1,3,1,6"));
    assertEquals(16ul, count_expanded_arrangements("????.#...#... 4,1,1"));
    assertEquals(2500ul, count_expanded_arrangements("????.######..#####. 1,6,5"));
    assertEquals(506250ul, count_expanded_arrangements("?###???????? 3,2,1"));

    const auto second_test_result = second(utils::split(test_raw_input));
    assertEquals(525152ul, second_test_result);
    cout << "Test 2 ok" << endl;

    const auto second_result = second(input);
    cout << "Second: " << second_result << endl;


    return EXIT_SUCCESS;
}
