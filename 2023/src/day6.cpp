#include "utils.h"
#include <numeric>

using namespace std;

const string test_raw_input = R"(Time:      7  15   30
Distance:  9  40  200)";

struct State{
    // In millisedonds
    vector<unsigned long> times;

    // In millimeters
    vector<unsigned long> distances;
};

unsigned long computeDistance(const unsigned long buttonDuration, const unsigned long raceTime) {
    const unsigned long remainingTime = raceTime - buttonDuration;
    return remainingTime * buttonDuration;
}


State parseState(const vector<string>& input) {
    State result;
    
    if(input.size() != 2) {
        throw runtime_error("Bad number of lines in the input");
    }

    auto currentLine = input.begin();
    {
        if(!currentLine->starts_with("Time:")){
            throw runtime_error("First line doesn't start with \"Time:\"");
        }
        istringstream iss{*currentLine};
        iss.ignore("Time:"s.length());
        iss >> result.times;
    }

    ++currentLine;
    {
        if(!currentLine->starts_with("Distance:")){
            throw runtime_error("First line doesn't start with \"Distance:\"");
        }
        istringstream iss{*currentLine};
        iss.ignore("Distance:"s.length());
        iss >> result.distances;
    }

    return result;
}



// brute-force
unsigned long bruteForceComputeBetterTimes(unsigned long raceTime, unsigned long bestDistance) {
    vector<unsigned long> computedDistances;
    for(size_t buttonTime = 0; buttonTime < raceTime; ++buttonTime) {
        computedDistances.emplace_back(computeDistance(buttonTime, raceTime));
    }

    auto betterDistanceStartsAt = 
        find_if(computedDistances.begin(), computedDistances.end(), [&bestDistance](unsigned long& distance){
            return distance > bestDistance;
        });

    auto betterDistanceEndsAt =
        find_if(computedDistances.rbegin(), computedDistances.rend(), [&bestDistance](unsigned long& distance){
            return distance > bestDistance;
        });
    
    long betterTimes = distance(betterDistanceEndsAt, make_reverse_iterator(betterDistanceStartsAt));

    return betterTimes;
}

long first(const vector<string>& input) {
    const auto state = parseState(input);

    vector<unsigned long> betterTimesPerPlay;

    for(size_t i = 0; i < state.times.size(); ++i) {
        const auto raceTime = state.times.at(i);
        const auto bestDistance = state.distances.at(i);

        long betterTimes = bruteForceComputeBetterTimes(raceTime, bestDistance);

        // cout << betterTimes << endl;
        betterTimesPerPlay.emplace_back(betterTimes);
    }

    auto result1 = accumulate(betterTimesPerPlay.cbegin(), betterTimesPerPlay.cend(), 1ul, multiplies<unsigned long>());

    return result1;
}

struct Race{
    unsigned long time;
    unsigned long bestDistance;
};

ostream& operator<< (ostream& os, const Race& race) {
    return os << "Race[time: " << race.time << ", bestDistance: " << race.bestDistance << " ]";
}

Race parseRace(const vector<string>& input) {
    Race result;
    if(input.size() != 2) {
        throw runtime_error("Bad number of lines in the input");
    }

    auto currentLine = input.begin();
    {
        string line = *currentLine;
        if(!line.starts_with("Time:")){
            throw runtime_error("First line doesn't start with \"Time:\"");
        }
        line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
        istringstream iss{line};
        iss.ignore("Time:"s.length());
        iss >> result.time;
    }

    ++currentLine;
    {
        string line = *currentLine;
        if(!line.starts_with("Distance:")){
            throw runtime_error("First line doesn't start with \"Distance:\"");
        }
        line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
        istringstream iss{line};
        iss.ignore("Distance:"s.length());
        iss >> result.bestDistance;
    }

    return result;
}

long second(const vector<string>& input) {
    const auto race = parseRace(input);
    // cout << race << endl;
    const auto bestTimes = bruteForceComputeBetterTimes(race.time, race.bestDistance);

    return bestTimes;
}

int main(const int, const char* []) {
    cout << "Day 6" << endl;

    const auto first_test_result = first(utils::split(test_raw_input));
    assertEquals(288, first_test_result);
    cout << "Test ok" << endl;

    const auto input = utils::readFile("input/day6");
    const auto first_result = first(input);
    cout << "First: " << first_result << endl;

    const auto second_test_result = second(utils::split(test_raw_input));
    assertEquals(71503, second_test_result);
    cout << "Test 2 ok" << endl;

    const auto second_result = second(input);
    cout << "Second: " << second_result << endl;

    return EXIT_SUCCESS;
}
