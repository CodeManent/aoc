#include "day.h"
#include "utils.h"

#include <string>

#include <map>
#include <numeric>
#include <stdexcept>
#include <numeric>

using namespace std;

class Day2Play {
    enum class Move {
        Rock,
        Paper,
        Scissors
    };

    enum class Outcome {
        Lose,
        Draw,
        Win
    };

    map<const char, Move> otherMoveMapping{
        {'A', Move::Rock},
        {'B', Move::Paper},
        {'C', Move::Scissors}
    };

    map<const char, Move> myMoveMapping{
        {'X', Move::Rock},
        {'Y', Move::Paper},
        {'Z', Move::Scissors}
    };

    map<const char, Outcome> outcomeMapping{
        {'X', Outcome::Lose},
        {'Y', Outcome::Draw},
        {'Z', Outcome::Win}
    };


    map<Move, Move> winsOver{
        {Move::Rock, Move::Scissors},
        {Move::Paper, Move::Rock},
        {Move::Scissors, Move::Paper}
    };

    map<Move, int> moveScore{
        {Move::Rock, 1},
        {Move::Paper, 2},
        {Move::Scissors, 3}
    };

    map<Outcome, int> outcomeScore{
        {Outcome::Lose, 0},
        {Outcome::Draw, 3},
        {Outcome::Win, 6}
    };

    Move otherPlay;
    Move myPlay;
    Outcome outcome;

    const Outcome computeOutcome() {
        if (myPlay == otherPlay) {
            return Outcome::Draw;
        }

        if (winsOver[myPlay] == otherPlay) {
            return Outcome::Win;
        }

        return Outcome::Lose;
    }

    const Move computeMyPLay() {
        if (outcome == Outcome::Draw) {
            return otherPlay;
        }

        if (outcome == Outcome::Lose) {
            return winsOver[otherPlay];
        }

        // find a losing move
        for (const auto& entry : winsOver) {
            if (entry.second == otherPlay) {
                return entry.first;
            }
        }

        throw exception("Couldn't compute an appropriate move");
    }

public:
    Day2Play(const string& playDef) {
        if (playDef.size() != 3 || playDef[1] != ' ') {
            throw std::exception("Not a valid play");
        }

        otherPlay = otherMoveMapping[playDef[0]];
        myPlay = myMoveMapping[playDef[2]];
        outcome = outcomeMapping[playDef[2]];
    }

    void fixOutcome() {
        outcome = computeOutcome();
    }

    void fixMyPLay() {
        myPlay = computeMyPLay();
    }

    const int score() {

        return moveScore[myPlay] + outcomeScore[outcome];
    }
};

DayResult day2() {
    const auto lines = readFile("day_2_input.txt");

    const int forwardResult = accumulate(lines.begin(), lines.end(), 0, [](int&& currentScore, const string& line) {
        Day2Play play(line);
    play.fixOutcome();
    const int lineScore = play.score();

    return currentScore + lineScore;
        });

    // cout << "Forward accumulated score: " << forwardResult << endl;

    const int reverseResult = accumulate(lines.begin(), lines.end(), 0, [](int&& currentScore, const string& line) {
        Day2Play play(line);
    play.fixMyPLay();
    const int lineScore = play.score();

    return currentScore + lineScore;
        }
    );
    // cout << "Reverse accumulated score: " << reverseResult << endl;

    return {
        make_optional<PartialDayResult>({"Forward accumulated score", to_string(forwardResult)}),
        make_optional<PartialDayResult>({"Reverse accumulated score", to_string(reverseResult)})
    };
}
