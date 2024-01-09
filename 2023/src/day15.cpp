#include "utils.h"
#include "numeric"
#include <array>
#include <list>

using namespace std;

const string test_raw_input = R"(rn=1,cm-,qp=3,cm=2,qp-,pc=4,ot=9,ab=5,pc-,pc=6,ot=7)";

// unsigned long hash(const string_view sv) {
//     unsigned long current_value = 0;
//     for(const char& ch : sv) {
//         current_value += static_cast<unsigned long>(ch);
//         current_value *= 17;
//         current_value %= 256;
//     }
// }

unsigned long HASH(const string_view sv) {
    unsigned char result = 0;
    for(const auto& ch: sv) {
        result += static_cast<unsigned char>(ch);
        result *= 17;
    }

    return result;
}

unsigned long first(const vector<string>& input) {
    return transform_reduce(
        cbegin(input),
        cend(input),
        0ul,
        plus(),
        HASH);
}

struct Lens{
    string id;
    enum class Operation: char{
        Set = '=',
        Remove = '-'
    } operation;
    unsigned long focalLength = 0;
};

ostream& operator<<(ostream& os, const Lens& lens) {
    os << lens.id;
    if(lens.operation == Lens::Operation::Remove){
        cout << '-';
    }
    else{
        cout << '=' << lens.focalLength;
    }
    return os;
}

Lens parseLens(const string_view input) {
    const auto pos = input.find_first_of("=-");
    if(pos == input.npos) {
        throw invalid_argument("Couldn't find separator for input: "s + string{input});
    }
    string id{input.substr(0, pos)};
    switch(input[pos]) {
        case '-':
            return Lens{move(id), Lens::Operation::Remove};
        case '=':
            return Lens{move(id), Lens::Operation::Set, utils::parseNumber<unsigned long>(input.substr(pos + 1))};
        default:
            throw runtime_error("Bad separator character: "s + input[pos]);
    }
}

long second(const vector<string>& input) {
    array<list<Lens>, 256> boxes;
    for(const auto& lens: input | views::transform(parseLens)) {
        const auto hash = HASH(lens.id);
        auto& box = boxes[hash];

        const auto matchesId = [&id = lens.id](const Lens& toCheck){
            return toCheck.id == id;
        };

        if(lens.operation == Lens::Operation::Remove){
            box.remove_if(matchesId);
        }
        else{
            const auto findPos = find_if(box.begin(), box.end(), matchesId);
            if(findPos == box.end()){
                box.push_back(lens);
            }
            else{
                *findPos = lens;
            }
        }
    }

    size_t totalFocusingPower = 0;

    size_t boxNum = 0;
    for(const auto& box: boxes) {
        ++boxNum;
        size_t elementNum = 0;
        for(const auto& element: box) {
            ++elementNum;
            const auto lensFocusingPower = boxNum * elementNum * element.focalLength;
            totalFocusingPower += lensFocusingPower;
        }
    }

    return totalFocusingPower;
}

int main(const int, const char* []) {
    cout << "Day 15" << endl;

    assertEquals(52u, HASH("HASH"));

    const auto splitTestInput = utils::split(test_raw_input, ',');
    const auto first_test_result = first(splitTestInput);
    assertEquals(1320ul, first_test_result);
    cout << "Test ok" << endl;

    const auto input = utils::readFile("input/day15");
    const auto splitInput = utils::split(input.at(0), ',');
    const auto first_result = first(splitInput);
    assertNotEquals(103ul, first_result, "Too low");
    cout << "First: " << first_result << endl;

    const auto second_test_result = second(splitTestInput);
    assertEquals(145, second_test_result);
    cout << "Test 2 ok" << endl;

    const auto second_result = second(splitInput);
    cout << "Second: " << second_result << endl;

    return EXIT_SUCCESS;
}
