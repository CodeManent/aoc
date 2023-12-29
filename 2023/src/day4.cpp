#include "utils.h"
#include <algorithm>
#include <iterator>
#include <ranges>
#include <optional>
#include <cmath>
#include <bitset>
#include <numeric>
#include <list>

using namespace std;

const string test_raw_input = R"(Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53
Card 2: 13 32 20 16 61 | 61 30 68 82 17 32 24 19
Card 3:  1 21 53 59 44 | 69 82 63 72 16 21 14  1
Card 4: 41 92 73 84 69 | 59 84 76 51 58  5 54 83
Card 5: 87 83 26 28 32 | 88 30 70 12 93 22 82 36
Card 6: 31 18 13 56 72 | 74 77 10 23 35 67 36 11)";

struct Card {
    long id;
    vector<long> winning;
    vector<long> numbers;

    size_t matching = 0;
    optional<unsigned long> worth;
    unsigned long multiplier = 1;
};

ostream& operator<<(ostream& os, const Card& card) {
    auto initialWidth = os.width();

    os << "Card " << os.width(3) << card.id << ":";
    // os.fill(' ');
    for(const auto& num : card.winning) {
        os.width(3);
        os << num;
    }
    os << " | ";
    for(const auto& num: card.numbers) {
        os.width(3);
        cout << num;
    }

    os << "| multiplier ";
    os.width(3);
    os << card.multiplier;

    os << " | matching ";
    os.width(3);
    os << card.matching;

    os << " worth " << (card.worth.has_value() ? to_string(card.worth.value()) : "nothing");

    os.width(initialWidth);

    return os;
}


Card parseCard(string input) {
    // cout << "Parsing line " << input << endl;
    istringstream iss{input};

    iss >> matchConsume("Card");

    Card result;
    iss >> result.id;

    iss >> matchConsume(':');

    {
        string winningStr;
        getline(iss, winningStr, '|');
        istringstream winningIss(winningStr);
        copy(
            istream_iterator<long>(winningIss),
            istream_iterator<long>(),
            back_inserter(result.winning));
    }

    string strNumbers;
    getline(iss, strNumbers);

    istringstream numbersIss{strNumbers};
    copy(
        istream_iterator<long>(numbersIss),
        istream_iterator<long>(),
        back_inserter(result.numbers));

    return result;
}

pair<long, long> compute(const vector<string>& input) {
    vector<Card> cards;
    list<long> pendingMultipliers;

    for(const auto& line: input) {
        auto card = parseCard(line);

        // compute card worth
        for(const auto number :card.numbers){
            if(find(begin(card.winning), end(card.winning), number) != end(card.winning)){
                ++card.matching;
            }
        }

        // cout << matching << " matching" << endl;
        if(card.matching){
            bitset<64> worth;
            worth.set(card.matching - 1);

            long computedWorth = static_cast<long>(worth.to_ulong());
            card.worth = make_optional(computedWorth);
        }




        // increate pending multipliers
        if(pendingMultipliers.empty()){
            card.multiplier = 1;
        }
        else{
            card.multiplier = pendingMultipliers.front();
            pendingMultipliers.pop_front();
        }

        size_t multipliersToFix = min(card.matching, pendingMultipliers.size());
        size_t multipliersToAdd = card.matching - multipliersToFix;

        auto listIterator = begin(pendingMultipliers);
        for(size_t i = 0; i < multipliersToFix; ++i) {
            *listIterator++ += card.multiplier;
        }

        for(size_t i = 0; i < multipliersToAdd; ++i) {
            pendingMultipliers.emplace_back(1 + card.multiplier);
        }

        cards.push_back(card);
    }

    // copy(cbegin(cards), cend(cards), ostream_iterator<Card>(cout, "\n"));

    const auto allWorth = transform_reduce(
        cbegin(cards), cend(cards),
         0,
         plus<long>(),
         [](const Card& card) {
            return card.worth.value_or(0);
        });

    const auto countCards = transform_reduce(
        begin(cards),
        end(cards),
        0,
        plus<unsigned long>(),
        [](const Card& card) {
            return card.multiplier;
        });

    return make_pair(allWorth, countCards);
}

// defined in teh extra file
int extraTestFunction();

int main(const int, const char* []) {
    cout << "Day 4" << endl;

    const auto [first_test_result, second_test_result] = compute(utils::split(test_raw_input));
    assertEquals(13, first_test_result);
    cout << "Test ok" << endl;

    const auto input = utils::readFile("input/day4");
    const auto [first_result, second_result] = compute(input);
    cout << "First: " << first_result << endl;

    assertEquals(30, second_test_result);
    cout << "Test 2 ok" << endl;

    cout << "Second: " << second_result << endl;

    return EXIT_SUCCESS;
}
