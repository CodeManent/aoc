#include "utils.h"
#include <ranges>
#include <optional>
#include <cmath>
#include <bitset>
#include <numeric>

using namespace std;

const string test_raw_input = R"(seeds: 79 14 55 13

seed-to-soil map:
50 98 2
52 50 48

soil-to-fertilizer map:
0 15 37
37 52 2
39 0 15

fertilizer-to-water map:
49 53 8
0 11 42
42 0 7
57 7 4

water-to-light map:
88 18 7
18 25 70

light-to-temperature map:
45 77 23
81 45 19
68 64 13

temperature-to-humidity map:
0 69 1
1 0 69

humidity-to-location map:
60 56 37
56 93 4)";

struct Extent{
    unsigned long base;
    unsigned long length;

    constexpr unsigned long end() const {
        return base + length;
    }

    constexpr bool hit(unsigned long number) const {
        return (base <= number) && (number <= (base + length-1));
    }

    constexpr pair<optional<Extent>, optional<Extent>> split(unsigned long number) const {
        if(number <= base) {
            return make_pair(nullopt, make_optional(*this));
        }
        if(end() < number) {
            return make_pair(make_optional(*this), nullopt);
        }
        const auto d = number - base;
        return make_pair(
            make_optional(Extent{base, d}),
            make_optional(Extent{number, length - d})
        );
    }
};

struct Mapping{
    unsigned long to;
    Extent from;

    constexpr bool hit(unsigned long number) const {
        return from.hit(number);
    }

    constexpr unsigned long map(const unsigned long& number) const {
        const bool hit = from.hit(number);
        if(hit) {
            unsigned long d = number - from.base;
            return to + d;
        }

        return number;
    }
};

struct MappingContainer{
    string name;
    vector<Mapping> mappings;

    constexpr optional<Mapping> hit(const unsigned long& number) const {
        auto found = find_if(begin(mappings), end(mappings), [&number](const Mapping& mapping) {
            return mapping.hit(number);
        });

        if(found == end(mappings)){
            return nullopt;
        }
        else{
            return make_optional(*found);
        }
    }

    constexpr unsigned long map(const unsigned long& number) const {
        if(const auto targetMap = hit(number); targetMap.has_value()){
            return targetMap->map(number);
        }
        else{
            return number;
        }
    }
    
    constexpr vector<Extent> project(const vector<Extent>& extents) const {
        vector<Extent> result;
        for(Extent extent: extents) {

            optional<Extent> remaining = make_optional(extent);

            // could possibly define and use a lower/upper bound instead of iterating all the mappings
            for(const auto mapping: mappings) {
                if(!remaining.has_value()){
                    break;
                }

                const auto& [before, baseRest] = remaining.value().split(mapping.from.base);
                if(before.has_value()){
                    // add to the result unmapped as it is before the mapping range
                    result.push_back(before.value());
                }

                remaining = baseRest;

                if(!remaining.has_value()){
                    // the extent has been exhausted
                    break;
                }

                const auto& [inRange, rest] = remaining.value().split(mapping.from.end());

                if(inRange.has_value()){
                    // only need to map the base.
                    const Extent mapped{
                        mapping.map(inRange.value().base),
                        inRange.value().length
                    };

                    result.emplace_back(mapped);
                }

                remaining = rest;
            }

            if(remaining.has_value()){
                // the remaining extent is after the mapping range. Add it unmapped to the result
                result.push_back(remaining.value());
            }
        }

        sort(result.begin(), result.end(), [](const Extent& lhs, const Extent& rhs){
            return lhs.base < rhs.base;
        });

        return result;
    }
};


struct State{
    vector<unsigned long> seeds;
    vector<MappingContainer> mappings;

    unsigned long seedToLocation(unsigned long seed) const {
        vector<unsigned int> path;
        path.emplace_back(seed);

        for(const auto &mapping : mappings){
            path.emplace_back(mapping.map(path.back()));
        }

        return path.back();
    }

    vector<Extent> seedsToLocations(vector<Extent> value) const {
        
        for(const auto& mapping: mappings){
            value = mapping.project(value);
        }

        return value;
    }
};

ostream& operator<<(ostream &os, const State& state) {
    os << "seeds: ";
    copy(state.seeds.begin(), state.seeds.end(), ostream_iterator<unsigned long>(os, " "));
    os << "\n";

    for(const auto& mc: state.mappings){
        os << "\n";
        os << mc.name << " map:\n";
        for(const auto& mapping: mc.mappings) {
            os << mapping.to << " " << mapping.from.base << " " << mapping.from.length << "\n";
        }
    }

    return os;
}



State parseState(const vector<string>&input) {
    State result;

    auto currentLine = input.begin();

    if(currentLine->size() < 4) {
        throw runtime_error("Not enough input lines");
    }

    // first line is the input seeds
    if(!currentLine->starts_with("seeds: ")){
        throw runtime_error("Parsing error 1: " + *currentLine);
    }

    istringstream iss(*currentLine);
    iss.ignore("seeds: "s.length());
    iss >> result.seeds;

    ++currentLine;
    if(*currentLine != ""){
        throw runtime_error("Parsing error 2: " + *currentLine);
    }

    while(currentLine != input.end()) {
        MappingContainer container;

        // consume empty line
        if(*currentLine != "") {
            throw runtime_error("Parsing error 3: " + *currentLine);
        }

        ++currentLine;
        istringstream iss{*currentLine};
        iss >> container.name;
        if(container.name + " map:" != *currentLine){
            throw runtime_error("Parsing error 4: " + *currentLine);
        }

        for(++currentLine; currentLine != input.end() && *currentLine != ""; ++currentLine) {
            vector<unsigned long> lineNumbers;

            istringstream iss{*currentLine};
            iss >> lineNumbers;
            container.mappings.emplace_back(lineNumbers[0], Extent{lineNumbers[1], lineNumbers[2]});
        }

        sort(container.mappings.begin(), container.mappings.end(), [](const Mapping& lhs, const Mapping& rhs) {
            return lhs.from.base < rhs.from.base;
        });

        result.mappings.emplace_back(container);
    }

    if(result.mappings.size() != 7) {
        throw runtime_error("Should have parsed 7 maps");
    }

    return result;
}

ostream& operator<< (ostream& os, const Extent& extent) {
    return os << "Extent[" << extent.base << ", " << extent.length << "]";
}

pair<long, long> compute(const vector<string>& input) {
    auto state = parseState(input);
    // cout << state << endl;

    vector<unsigned int> locations;
    for(const auto& seed: state.seeds) {
        locations.emplace_back(state.seedToLocation(seed));
    }

    // print(locations);
    const auto result1Iter = min_element(locations.begin(), locations.end());

    // brute-force, takes too long
    // for(const auto& seedRange : seedRanges){
    //     for(const auto& seed: views::iota(seedRange.base, seedRange.base + seedRange.length)){
    //         locations2.emplace_back(state.seedToLocation(seed));
    //     }
    // }

    vector<Extent> seedRanges;
    for(size_t i = 0; i < state.seeds.size(); i += 2) {
        seedRanges.emplace_back(Extent{state.seeds[i], state.seeds[i + 1]});
    }
    // print(seedRanges);
    const auto locationRanges = state.seedsToLocations(seedRanges);
    // since all the extents are monotonically increating, only the base is interesting to use
    auto locations2 = locationRanges
        | views::transform([](const Extent& locationExtent) {
            return locationExtent.base;
        });

    const auto resultIter2 = min_element(locations2.begin(), locations2.end());

    return make_pair(*result1Iter, *resultIter2);
}

int main(const int, const char* []) {
    cout << "Day 5" << endl;

    const auto [first_test_result, second_test_result] = compute(utils::split(test_raw_input));
    assertEquals(35, first_test_result);
    cout << "Test ok" << endl;

    assertEquals(46, second_test_result);
    cout << "Test 2 ok" << endl;

    const auto input = utils::readFile("input/day5");
    const auto [first_result, second_result] = compute(input);
    cout << "First: " << first_result << endl;

    cout << "Second: " << second_result << endl;

    return EXIT_SUCCESS;
}
