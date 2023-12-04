#include "utils.h"
#include <algorithm> // max
#include <ranges>
#include <numeric> // accumulate

using namespace std;

const string test_raw_input = R"(Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green)";

struct Play{
    size_t red = 0;
    size_t green = 0;
    size_t blue = 0;

    size_t power() const {
        return red == 0 && green == 0 && blue == 0 ? 0:
            max(1UL, red) * max(1UL, green) * max(1UL, blue);
    }
};

struct Game{
    size_t id = 0;
    vector<Play> plays;
};

ostream& operator<<(ostream& os , const Play& play) {
    bool first = true;

    if(play.red) {
        os << play.red << " red";
        first = false;
    }
    if(play.green) {
        if(!first) {
            os << ", ";
        }
        os << play.green << " green";
        first = false;
    }

    if(play.blue){
        if(!first) {
            os << ", ";
        }
        os << play.blue << " blue";
        first = false;
    }

    return os;
}

ostream& operator<<(ostream& os, const Game& game) {
    return os << "Game " << game.id << ": " << game.plays;
}

Play parsePlay(const string& input) {
    // cout << "parsePlay: " << input << endl;

    istringstream iss(input);
    Play result;

    int count;
    string colour;
    int foundColors;
    while(!iss.eof()) {
        string colourDef;
        getline(iss, colourDef, ',');
        // cout << "colourDef: " << colourDef << endl;
        istringstream coloutIss(colourDef);
        coloutIss >> count;
        coloutIss >> colour;
        // cout << count << " " << colour << endl;

        if(colour == "red") {
            result.red = count;
        }
        else if(colour == "green") {
            result.green = count;
        }
        else if(colour == "blue") {
            result.blue = count;
        }
        else{
            throw runtime_error(string{"Unknown colour definition: " } + colour);
        }
        ++foundColors;
    }

    if(!foundColors) {
        throw runtime_error(string{"Couldn't parse any color from input: "} + input);
    }

    return result;
}

Game parseGame(const string& input) {
    istringstream iss(input);
    string header;
    iss >> header;
    if(header != "Game"){
        throw runtime_error("Not a valid header");
    }

    Game result{};
    iss >> result.id;
    char separator1;
    iss >> separator1;
    if(separator1 != ':'){
        throw runtime_error("Bad game-plays separator");
    }

    while(!iss.eof()) {
        string playLine;
        getline(iss, playLine, ';');

        result.plays.emplace_back(parsePlay(playLine));
    }

    return result;
}

const Play gameLimit{
    12,13, 14
};

long first(vector<string> input) {
    auto validGames = input
        | views::transform(parseGame)
        | views::filter([](const Game& game) {
            auto invalidPlays = game.plays
                | views::filter([](const Play& play) {
                    return play.red > gameLimit.red
                        || play.green > gameLimit.green
                        || play.blue > gameLimit.blue;
                });
            return invalidPlays.empty();
        });

    auto ids = validGames
        | views::transform([](const Game& game) {
            return game.id;
        });

    return accumulate(ids.begin(), ids.end(), 0);
}

long second(vector<string> input) {
    auto powers = input
        | views::transform(parseGame)
        | views::transform([](const Game& game) {
            return accumulate(
                game.plays.cbegin(),
                game.plays.cend(),
                Play{},
                [](const Play& lhs, const Play& rhs) {
                    return Play{
                        max(lhs.red, rhs.red),
                        max(lhs.green, rhs.green),
                        max(lhs.blue, rhs.blue)
                    };
                });
        })
        | views::transform([](const auto& elem) {
            return elem.power();
        })
        ;
    
    return accumulate(cbegin(powers), cend(powers), 0);
}

int main(const int, const char* []) {
    cout << "Day 2" << endl;

    const auto first_test_result = first(utils::split(test_raw_input));
    assertEquals(8, first_test_result);
    cout << "Test ok" << endl;

    const auto input = utils::readFile("input/day2");
    const auto first_result = first(input);
    cout << "First: " << first_result << endl;

    const auto second_test_result = second(utils::split(test_raw_input));
    assertEquals(2286, second_test_result);
    cout << "Test 2 ok" << endl;

    const auto second_result = second(input);
    cout << "Second: " << second_result << endl;

    return EXIT_SUCCESS;
}