#include "day.h"

#include "ParsingError.h"

#include <iostream>
#include <queue>
#include <deque>
#include <fstream>
#include <sstream>
#include <iostream>
#include <charconv>
#include <numeric>

using namespace std;

class Monkey {
public:
	int id;
	deque<unsigned long long> items;

	unsigned long long lhs;
	unsigned long long rhs;
	char op;

	unsigned long long testDivider;
	int trueTarget;
	int falseTarget;

	unsigned long long itemInspections = 0;
};

ostream& operator<<(ostream& os, const Monkey& m) {
	os << "Monkey " << m.id << endl;
	os << "  Starting items:";
	bool first = true;

	for (const auto item : m.items) {
		if (first) {
			first = false;
		}
		else {
			os << ',';
		}
		os << ' ' << item;
	}
	os << endl;

	os << "  Operation: " << m.lhs << " " << m.op << " " << m.rhs << endl;
	os << "  Test: divisible by " << m.testDivider << endl;
	os << "    If true: throw to monkey " << m.trueTarget << endl;
	os << "    If false: throw to monkey " << m.falseTarget << endl;

	return os;
}

template <class _Elem, class _Traits>
basic_istream<_Elem, _Traits>& operator>>(basic_istream<_Elem, _Traits>& is, const string&& pin) {
	string input;
	is >> input;
	if (pin != input) {
		throw ParsingError("The input [" + input + "] does not equal [" + pin + "]");
	}

	return is;
}

Monkey parseMonkey(ifstream& infile) {
	Monkey result;

	infile >> "Monkey" >> result.id >> ":";

	char numbers[1024];
	infile >> "Starting" >> "items:";

	infile.getline(numbers, 1023, '\n');
	istringstream iss(numbers);
	while (iss && !iss.eof()) {
		if (!result.items.empty()) {
			iss >> ",";
		}
		int value;
		iss >> value;
		result.items.push_back(value);
	}

	string lhs, rhs;
	char op;

	infile >> "Operation:" >> "new" >> "=" >> lhs >> op >> rhs;
	if (lhs == "old") {
		result.lhs = 0;
	}
	else if(const auto res = from_chars(lhs.data(), lhs.data() + lhs.size(), result.lhs); res.ptr != lhs.data() + lhs.size()) {
		throw ParsingError("[" + lhs + "] is not trivially converted to a number");
	}

	if (rhs == "old") {
		result.lhs = 0;
	}
	else if (const auto res = from_chars(rhs.data(), rhs.data() + rhs.size(), result.lhs); res.ptr != rhs.data() + rhs.size()) {
		throw ParsingError("[" + rhs + "] is not trivially converted to a number");
	}
	result.op = op;

	infile >> "Test:" >> "divisible" >> "by" >> result.testDivider;
	infile >> "If" >> "true:" >> "throw" >> "to" >> "monkey" >> result.trueTarget;
	infile >> "If" >> "false:" >> "throw" >> "to" >> "monkey" >> result.falseTarget;

	return result;
}

bool hasInput(ifstream& is) {
	while (is) {
		const auto ch = is.get();
		if (ch == istream::traits_type::eof()) {
			return false;
		}
		if (!isspace(ch)) {
			is.putback(ch);
			return true;
		}
	}
	return false;
}

vector<Monkey> parseMonkeys(string filename) {
	vector<Monkey> result;
	ifstream infile(filename);
	while (infile && hasInput(infile)) {
		// cout << "Reading monkey " << result.size() << endl;
		result.push_back(parseMonkey(infile));
	}
	
	return result;
}

DayResult day11() {
	unsigned long long monkeyBusiness[2] = {};
	long roundsPerPart[2] = {20, 10000};

	for (int part = 1; part <= 2; ++part) {

		auto monkeys = parseMonkeys("day_11_input.txt");
		auto commonMultiplier = accumulate(monkeys.cbegin(), monkeys.cend(), monkeys.cbegin()->testDivider, [](const unsigned long long accumulated, const Monkey& rhs) {
			return lcm(accumulated, rhs.testDivider);
			});
		// cout << "commonMultiplier : " << commonMultiplier << endl;

		// process rounds
		for (int round = 1; round <= roundsPerPart[part-1]; ++round) {
			for (Monkey& monkey : monkeys) {
				//cout << monkey << endl;

				// cout << "Monkey " << monkey.id << endl;

				for (auto item : monkey.items) {
					//cout << "  Monkey inspects an item with a worry level of " << item << endl;
					++monkey.itemInspections;


					switch (monkey.op) {
					case '*':
						item = (monkey.lhs ? monkey.lhs : item) * (monkey.rhs ? monkey.rhs : item);
						//cout << "    Worry level is multiplied by " << rhsValue << " to " << item << '.' << endl;
						break;
					case '+':
						item = (monkey.lhs ? monkey.lhs : item) + (monkey.rhs ? monkey.rhs : item);
						//cout << "    Worry level is increased by " << rhsValue << " to " << item << '.' << endl;
						break;
					default:
						throw runtime_error(string(1, monkey.op) + " is not a valid operator");
					}

					if (part == 1) {
						item /= 3;
						//cout << "    Monkey gets bored with item.Worry level is divided by 3 to " << item << '.' << endl;
					}
					else {
						item %= commonMultiplier;
					}

					int targetMonkey = item % monkey.testDivider == 0 ? monkey.trueTarget : monkey.falseTarget;

					//cout << "    Item with worry level " << item << " is thrown to monkey " << targetMonkey << '.' << endl;
					monkeys[targetMonkey].items.push_back(item);
				}
				monkey.items.clear();
			}

			/*
			cout << "After round " << round << ", the monkeys are holding items with these worry levels:" << endl;
			for (const auto& monkey : monkeys) {
				cout << "Monkey " << monkey.id << ":";
				bool notFirst = false;
				for (const auto item : monkey.items) {
					if (notFirst) {
						cout << ',';
					}
					cout << ' ' << item;
					notFirst = true;
				}
				cout << endl;
			}
			cout << endl;
			*/
			/*
			if (round == 1 or round == 20 or round % 1000 == 0) {
				cout << "== After round " << round << " ==" << endl;
				for (const auto& monkey : monkeys) {
					cout << "Monkey " << monkey.id << " inspected items " << monkey.itemInspections << " times." << endl;
				}
				cout << endl;
			}
			*/
		}


		const Monkey* maxMonkey = &monkeys[0];
		for (const auto& monkey : monkeys) {
			if (monkey.itemInspections > maxMonkey->itemInspections) {
				maxMonkey = &monkey;
			}
		}

		const Monkey* secondMax = maxMonkey == &monkeys[0] ? &monkeys[1] : &monkeys[0];
		for (const auto& monkey : monkeys) {
			if (maxMonkey != &monkey && monkey.itemInspections > secondMax->itemInspections) {
				secondMax = &monkey;
			}
		}

		/*
		cout << "Max monkey: " << maxMonkey->id << " with value << " << maxMonkey->itemInspections << endl;
		cout << "Second monkey: " << secondMax->id << " with value << " << secondMax->itemInspections << endl;
		cout << endl;
		*/

		monkeyBusiness[part-1] = maxMonkey->itemInspections * secondMax->itemInspections;
	}

	return {
		make_optional<PartialDayResult>({"Initial monkey business", to_string(monkeyBusiness[0])}),
		make_optional<PartialDayResult>({"Second monkey business", to_string(monkeyBusiness[1])})
	};
}