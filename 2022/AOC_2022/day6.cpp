#include "day.h"

#include "ParsingError.h"

#include <cassert>
#include <fstream>

using namespace std;

size_t find_unique(const string& input, const size_t window) {
    // check if window is ok. if not, push it after the last matching pair

    for (size_t w = window - 1; w < input.size(); ) {
        for (size_t x = 0; x < window - 1; ++x) {
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

DayResult day6() {
    assert(startOfPacket("bvwbjplbgvbhsrlpgdmjqwftvncz") == 5);
    assert(startOfPacket("nppdvjthqldpwncqszvftbrmjlhg") == 6);
    assert(startOfPacket("nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg") == 10);
    assert(startOfPacket("zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw") == 11);

    ifstream infile("day_6_input.txt");
    std::string input{ istream_iterator<char>(infile), istream_iterator<char>() };

    const auto packetStart = startOfPacket(input);

    assert(startOfMessage("mjqjpqmgbljsphdztnvjfqwrcgsmlb") == 19);
    assert(startOfMessage("bvwbjplbgvbhsrlpgdmjqwftvncz") == 23);
    assert(startOfMessage("nppdvjthqldpwncqszvftbrmjlhg") == 23);
    assert(startOfMessage("nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg") == 29);
    assert(startOfMessage("zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw") == 26);

    const auto messageStart = startOfMessage(input);

    return {
        make_optional<PartialDayResult>({"Start of packet", to_string(packetStart)}),
        make_optional<PartialDayResult>({"Start of message", to_string(messageStart)})
    };
}
