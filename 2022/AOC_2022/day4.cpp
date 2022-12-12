#include "day.h"
#include "utils.h"

#include <charconv>
#include <stdexcept>
#include <string_view>

using namespace std;

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

    //friend ostream& operator<<(ostream&, const Range&);
};
//
//ostream& operator<<(ostream& os, const Range& r) {
//    os << '[' << r.start << '-' << r.end << ']';
//
//    return os;
//}

Range parseRange(const string_view& input) {
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

DayResult day4() {
    const auto lines = readFile("day_4_input.txt");

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

    return {
        make_optional<PartialDayResult>({"Fully contined ranges count", to_string(fullyContainedSum)}),
        make_optional<PartialDayResult>({"Overlapping ranges", to_string(overlapsSum)})
    };
}
