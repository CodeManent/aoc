#ifndef _days_h_
#define _days_h_

#include <string>
#include <optional>

struct PartialDayResult {
	std::string description;
	std::string value;
};

struct DayResult {
	std::optional<PartialDayResult> part1;
	std::optional<PartialDayResult> part2;
};

DayResult day1();
DayResult day2();
DayResult day3();
DayResult day4();
DayResult day5();
DayResult day6();
DayResult day7();
DayResult day8();

#endif
