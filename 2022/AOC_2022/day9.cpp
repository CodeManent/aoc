#include "day.h"
#include "utils.h"
#include "ParsingError.h"

#include "Coords.h"
#include "Rope.h"
#include "printers.h"

using namespace std;

#include <sstream>

const string to_string(const Coords& c) {
	ostringstream ss;
	ss << c;
	return ss.str();
}

#include <algorithm>
#include <ranges>
#include <iostream>
#include <functional>

void printState(size_t width, size_t height, const Coords& origin, const Rope& rope, const auto trailBegin, const auto trailEnd) {
	vector<vector<char>> field(height, vector<char>(width, '.'));

	const auto put = [& origin, &field](const Coords& loc, const char ch) {
		const Coords mapped { loc.x + origin.x, -loc.y + origin.y };

		field[mapped.y][mapped.x] = ch;
	};

	put({ 0, 0 }, 'S');

	// put train
	std::for_each(trailBegin, trailEnd, [&put](const Coords& c) {
		put(c, '#');
	});

	// put tail
	for (size_t i = rope.knots.size() - 1; i > 0; --i) {
		put(rope.knots.at(i), static_cast<char>(i + '0'));
	}

	// put head
	put(rope.first(), 'H');

	ranges::for_each(field, [](const auto& line) {
		ranges::copy(line, ostream_iterator<char>(cout));
		cout << endl;
	});
}

#include <unordered_set>

DayResult day9() {
	const auto& lines = readFile("day_9_input.txt");

	Rope rope{ 10, { 0, 0 } };

	auto coordsHashFunc = [](const Coords& c) -> size_t {
		return static_cast<size_t>(static_cast<uint64_t>(static_cast<uint32_t>(c.x)) << 32 | static_cast<uint32_t>(c.y));
	};

	unordered_set<Coords, decltype(coordsHashFunc)> visitedByTail;
	unordered_set<Coords, decltype(coordsHashFunc)> visitedByTail2;

	for (const auto& line : lines) {

		int multiplier = atoi(line.data() + 2);
		//cout << "== " << line << " ==\n\n";

		while (multiplier --> 0) {
			switch (line[0]) {
			case 'L': rope.move(Left); break;
			case 'R': rope.move(Right); break;
			case 'U': rope.move(Up); break;
			case 'D': rope.move(Down); break;
			default:
				throw ParsingError("Bad direction specifier");
			}

			visitedByTail.insert(rope.second());
			visitedByTail2.insert(rope.last());
			//printState(6, 5, { 0, 4 }, rope, visitedByTail2.cbegin(), visitedByTail2.cend());
			//cout << endl;
		}

		//printState(27, 21, { 11, 15 }, rope, visitedByTail2.cbegin(), visitedByTail2.cend());
		//cout << endl;
	}


	return {
		make_optional<PartialDayResult>({"Visited by small tail", to_string(visitedByTail.size())}),
		make_optional<PartialDayResult>({"visited by full tail", to_string(visitedByTail2.size())}),
	};
}
