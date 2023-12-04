#include "utils.h"

using namespace std;

const string test_raw_input = R"(line1
line2
line3
line4)";

long first(const vector<string>& input) {
    return input.size();
}

long second(const vector<string>& input) {
    return input.size();
}

// defined in teh extra file
int extraTestFunction();

int main(const int, const char* []) {
    cout << "Day Template" << endl;

    const auto first_test_result = first(utils::split(test_raw_input));
    assertEquals(4, first_test_result);
    cout << "Test ok" << endl;

    const auto input = utils::readFile("input/example");
    const auto first_result = first(input);
    cout << "First: " << first_result << endl;

    const auto second_test_result = second(utils::split(test_raw_input));
    assertEquals(4, second_test_result);
    cout << "Test 2 ok" << endl;

    const auto second_result = second(input);
    cout << "Second: " << second_result << endl;

    cout << "Value from extra source function call: " << extraTestFunction() << endl;

    return EXIT_SUCCESS;
}
