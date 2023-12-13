#include "utils.h"
#include <array>
#include <ranges>
#include <map>

using namespace std;

const string test_raw_input = R"(32T3K 765
T55J5 684
KK677 28
KTJJT 220
QQQJA 483
)";

enum class Card: unsigned int{
    A = 14, K=13, Q = 12, J = 11, T = 10, C9 = 9, C8 = 8, C7 = 7, C6 = 6, C5 = 5, C4 = 4, C3 = 3, C2 = 2,
};

enum class JokerCard: unsigned int{
    A = 14, K=13, Q = 12, T = 10, C9 = 9, C8 = 8, C7 = 7, C6 = 6, C5 = 5, C4 = 4, C3 = 3, C2 = 2, J = 1
};

enum class Kind: unsigned int {
    Kind5 = 50,
    Kind4 = 40,
    FullHouse = 32,
    Kind3 = 30,
    Kind22 = 22,
    Kind2 = 20,
    HighCard = 10
};

template<typename CardType>
struct Hand{
    array<CardType, 5> cards;
    Kind kind;

    bool operator<(const Hand<CardType>& other) const {
        if(other.kind == kind){
            const auto pos = ranges::mismatch(cards, other.cards);
            return *(pos.in1) < *(pos.in2);
        }
        else{
            return kind < other.kind;
        }
    }
};

Kind computeKind(const array<Card, 5>& cards) {
    auto c = cards;
    sort(c.begin(), c.end());
    map<Card, unsigned long> histogram;
    for(const auto& card: cards) {
        histogram[card] +=1;
    }

    vector<pair<Card, unsigned int>> histValues(histogram.begin(), histogram.end());
    sort(histValues.rbegin(), histValues.rend(), [](const auto& lhs, const auto& rhs) {
        return lhs.second < rhs.second;
    });

    switch(histValues[0].second){
        case 5: return Kind::Kind5;
        case 4: return Kind::Kind4;
        case 3: return histValues[1].second == 2 ? Kind::FullHouse : Kind::Kind3;
        case 2: return histValues[1].second == 2 ? Kind::Kind22 : Kind::Kind2;
        case 1: return Kind::HighCard;
        default:
            throw runtime_error("Unfeasible value for number if card appearences in histogram");
    }
}

Kind computeKind(const array<JokerCard, 5>& cards) {
    auto c = cards;
    sort(c.begin(), c.end());
    map<JokerCard, unsigned long> histogram;
    for(const auto& card: cards) {
        histogram[card] +=1;
    }

    const size_t jokers = histogram[JokerCard::J];
    if(jokers == 5) {
        return Kind::Kind5;
    }
    histogram.erase(JokerCard::J);
    
    vector<pair<JokerCard, unsigned int>> histValues(histogram.begin(), histogram.end());
    sort(histValues.rbegin(), histValues.rend(), [](const auto& lhs, const auto& rhs) {
        return lhs.second < rhs.second;
    });



    switch(histValues[0].second + jokers){
        case 5: return Kind::Kind5;
        case 4: return Kind::Kind4;
        case 3: return histValues[1].second == 2 ? Kind::FullHouse : Kind::Kind3;
        case 2: return histValues[1].second == 2 ? Kind::Kind22 : Kind::Kind2;
        case 1: return Kind::HighCard;
        default:
            throw runtime_error("Unfeasible value for number if card appearences in histogram");
    }
}

template <typename TCard>
struct Play{
    Hand<TCard> hand;
    unsigned long bid;
};

ostream& operator<<(ostream& os, const Card& card) {
    using enum Card;

    char ch;
    switch(card){
        case A: ch = 'A'; break;
        case K: ch = 'K'; break;
        case Q: ch = 'Q'; break;
        case J: ch = 'J'; break;
        case T: ch = 'T'; break;
        case C9: ch = '9'; break;
        case C8: ch = '8'; break;
        case C7: ch = '7'; break;
        case C6: ch = '6'; break;
        case C5: ch = '5'; break;
        case C4: ch = '4'; break;
        case C3: ch = '3'; break;
        case C2: ch = '2'; break;
        default:
            throw runtime_error("Unhandled card");
    }
    return os << ch;
}

ostream& operator<<(ostream& os, const JokerCard& card) {
    using enum JokerCard;

    char ch;
    switch(card){
        case A: ch = 'A'; break;
        case K: ch = 'K'; break;
        case Q: ch = 'Q'; break;
        case J: ch = 'J'; break;
        case T: ch = 'T'; break;
        case C9: ch = '9'; break;
        case C8: ch = '8'; break;
        case C7: ch = '7'; break;
        case C6: ch = '6'; break;
        case C5: ch = '5'; break;
        case C4: ch = '4'; break;
        case C3: ch = '3'; break;
        case C2: ch = '2'; break;
        default:
            throw runtime_error("Unhandled card");
    }
    return os << ch;
}


template <typename TCard>
ostream& operator<<(ostream&os, const Play<TCard>& play) {
    return os << play.hand.cards[0]
    << play.hand.cards[1]
    << play.hand.cards[2]
    << play.hand.cards[3]
    << play.hand.cards[4]
    << ' '
    << "- "
    << static_cast<unsigned int>(play.hand.kind)
    << " - "
    << play.bid;
}

template <typename TCard>
TCard parseCard(const char& ch) {

    switch(ch) {
        case 'A': return TCard::A;
        case 'K': return TCard::K; 
        case 'Q': return TCard::Q;
        case 'J': return TCard::J;
        case 'T': return TCard::T;
        case '9': return TCard::C9;
        case '8': return TCard::C8;
        case '7': return TCard::C7;
        case '6': return TCard::C6;
        case '5': return TCard::C5;
        case '4': return TCard::C4;
        case '3': return TCard::C3;
        case '2': return TCard::C2;
        default:
            throw runtime_error("Parsing error: Unknown card: " + ch);
    }
}

template <typename TCard>
Play<TCard> parsePlay(const string& input) {
    Play<TCard> result;

    for(size_t i = 0; i < 5; ++i) {
        const auto& card = parseCard<TCard>(input[i]);
        result.hand.cards[i] = card;
    }

    result.hand.kind = computeKind(result.hand.cards);

    result.bid = stoul(input.substr(6));

    return result;
}


template <typename TCard>
unsigned long computeWinnings(const vector<string>& input) {
    vector<Play<TCard>> plays;
    plays.reserve(input.size());

    transform(cbegin(input), cend(input), back_inserter(plays), parsePlay<TCard>);
    sort(begin(plays), end(plays), [](const auto& lhs, const auto& rhs) {
        return lhs.hand < rhs.hand;
    });

    unsigned long winnings = 0;
    for(size_t i = 0; i < plays.size(); ++i) {
        winnings += (i+1) * plays[i].bid;
    }

    // print(plays);


    return winnings;
}

long first(const vector<string>& input) {
    return computeWinnings<Card>(input);
}

long second(const vector<string>& input) {
    return computeWinnings<JokerCard>(input);
}

int main(const int, const char* []) {
    cout << "Day 7" << endl;

    const auto first_test_result = first(utils::split(test_raw_input));
    assertEquals(6440, first_test_result);
    cout << "Test ok" << endl;

    const auto input = utils::readFile("input/day7");
    const auto first_result = first(input);
    cout << "First: " << first_result << endl;

    const auto second_test_result = second(utils::split(test_raw_input));
    assertEquals(5905, second_test_result);
    cout << "Test 2 ok" << endl;

    const auto second_result = second(input);
    cout << "Second: " << second_result << endl;

    return EXIT_SUCCESS;
}
