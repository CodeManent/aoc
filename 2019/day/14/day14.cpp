#include <iostream>
#include <cstdlib>

#include <string>
#include <iterator>
#include <vector>
#include <algorithm>
#include <regex>
#include <optional>
#include <utility>


struct Chemical {
	std::string element;
	long long quantity;

	std::string to_string(long long remove = 0) const {
		return std::to_string(quantity-remove) + " " + element;
	}
};

struct Reaction {
	std::vector<Chemical> input;
	Chemical output;

	std::string to_string() const {
		std::stringstream ss;
		bool first = true;
		for(const auto &i : input){
			if(!first){
				ss << ", ";
			}
			first = false;
			ss << i.to_string();
		}
		ss << " => ";
		ss << output.to_string();

		return ss.str();
	}
};

typedef std::vector<Reaction> Reactions;

Chemical ParseChemical(const std::string &input){
	Chemical result;
	std::istringstream ss(input);
	if(ss.peek() == ','){
		ss.ignore(1);
	}
	ss >> result.quantity;
	if(!ss) goto prod_error;

	if(ss.get() != ' ') goto prod_error;

	ss >> result.element;
	if(!ss) goto prod_error;

	return result;

prod_error:
	throw std::runtime_error("Invalid Chemical input: " + input);
}

std::optional<Reaction> ParseReaction(const std::string &line){
	if(line.size() == 0){
		return std::nullopt;
	}
	// 7 A, 1 B => 1 C
	// 2 AB, 3 BC, 4 CA => 1 FUEL
	const std::regex itemRx{R"(\d+ [A-Z]+)"};
	const std::regex reactRx{R"(^((,? ?\d+ [A-Z]+)+) => (\d+ [A-Z]+)$)"};
	std::smatch match;

	if(!std::regex_match(line, match, reactRx)){
		std::cerr << "Couldn't match line: " << line << std::endl;
		return std::nullopt;
	}
	if(match.size() != 4){
		std::cerr << "Bad number of matches (" << match.size() << ") for " << line << std::endl;
		return std::nullopt;
	}

	Reaction result;

	auto iter = std::sregex_iterator(match[1].first, match[1].second, itemRx);
	for(;iter != std::sregex_iterator(); ++iter){
		result.input.push_back(ParseChemical(iter->str()));
	}

	result.output = ParseChemical(match[3].str());

	return result;
}

Reactions ParseInput(std::istream &&is){
	Reactions result;

	while (is) {
		std::string line;
		std::getline(is, line);
		if(const auto r = ParseReaction(line); r.has_value()){
			result.push_back(r.value());
		}
	}

	return result;
}

Reactions ParseInput(const std::string &input){
	return ParseInput(std::istringstream{input});
}


long long OreRequirement(const Chemical &target, const Reactions &reactions) {
	// std::map<std::string, BufferEntry> buffer;

	// buffer[target.element].reserved += target.quantity;
	// return Produce(target, reactions, buffer, log);
	Chemical ore{"ORE", 0};

	std::deque<Chemical> requirements;
	std::vector<Chemical> extra;

	requirements.emplace_back(target);

	//const auto notOre = [](const auto &ch) { return ch.element != "ORE"; };

	while (requirements.size())
	{
		// select element
		const auto current = requirements.front();
		requirements.pop_front();

		long long toProduce = current.quantity;

		// consume extra
		const auto extraIter = std::find_if(extra.begin(), extra.end(), [&](const auto &e) {
			return e.element == current.element;
		});
		if(extraIter != extra.end()){
			if(extraIter->quantity >= toProduce){
				extraIter->quantity -= toProduce;
				continue;
			}
			else{
				toProduce -= extraIter->quantity;
				extraIter->quantity = 0;
			}
		}

		// find reaction
		const auto reaction = *std::find_if(reactions.cbegin(), reactions.cend(), [&](const auto &r){
			return r.output.element == current.element;
		});

		long long multiplier = (toProduce / reaction.output.quantity) + (toProduce % reaction.output.quantity ? 1 : 0);

		for( const auto &input: reaction.input){
			if(input.element == ore.element){
				ore.quantity += input.quantity * multiplier;
				continue;
			}

			// not ore;
			auto iter = std::find_if(requirements.begin(), requirements.end(), [&](const auto &ch){
				return ch.element == input.element;
			});

			if(iter != requirements.end()){
				iter->quantity += input.quantity * multiplier;
			}
			else{
				requirements.emplace_back(Chemical{input.element, input.quantity * multiplier});
			}
		}

		long long remaining = reaction.output.quantity * multiplier - toProduce;
		if(remaining){
			if(extraIter != extra.end()){
				extraIter->quantity += remaining;
			}
			else{
				extra.emplace_back(Chemical{current.element, remaining});
			}

		}

	}

	return ore.quantity;
}

long long FuelFromTrillionOre(const Reactions &reactions)
{
	long long maxOre = 1000000000000ll;

	auto calculateOre = [&](const auto target) {
		return OreRequirement({"FUEL", target}, reactions);
	};

	// find min-max

	long long min = 1;
	long long max = 1;
	while(calculateOre(max<<=1) < maxOre)
	{
		min = max;
	}

	// search between min-max
	while (min != max) {
		auto target = (min + max)/2;
		auto oreAmount = calculateOre(target);

		if (oreAmount == maxOre){
			return target;
		}
		else if (oreAmount < maxOre)
		{
			min = target;
		}
		else {
			max = target;
		}
		if(max - min == 1){
			break;
		}
	}

	return min;
}










bool Test(){
	if (not ParseReaction("10 ORE => 10 A").has_value()){
		std::cerr << "Couldn't parse test reaction 1." << std::endl;
		return false;
	}

	if (const auto r = ParseReaction("7 A, 1 E => 1 FUEL"); not r.has_value() or  r.value().input.size() != 2)
	{
		std::cerr << "Couldn't parse test reaction 2." << std::endl;
		return false;
	}

	if(const auto r = ParseReaction("2 AB, 3 BC, 4 CA => 1 FUEL"); not r.has_value() or r.value().input.size() != 3){
		std::cerr << "Couldn't parse test reaction 3." << std::endl;
		return false;
	}

	const auto input1 = R"(
10 ORE => 10 A
1 ORE => 1 B
7 A, 1 B => 1 C
7 A, 1 C => 1 D
7 A, 1 D => 1 E
7 A, 1 E => 1 FUEL)";
	const auto reactions1 = ParseInput(input1);
	// for (const auto &r : reactions1) {
	// 	std::cout << r.to_string() << std::endl;
	// }
	const auto result1 = OreRequirement({"FUEL", 1}, reactions1);
	if(result1 != 31){
		std::cerr << "Bad result1 " << result1 << " was expecting 31." << std::endl;
		return false;
	}

	const auto input2 = R"(
9 ORE => 2 A
8 ORE => 3 B
7 ORE => 5 C
3 A, 4 B => 1 AB
5 B, 7 C => 1 BC
4 C, 1 A => 1 CA
2 AB, 3 BC, 4 CA => 1 FUEL)";
	const auto reactions2 = ParseInput(input2);
	// for (const auto &r : reactions2) {
	// 	std::cout << r.to_string() << std::endl;
	// }
	const auto result2 = OreRequirement({"FUEL", 1}, reactions2);
	if(result2 != 165){
		std::cerr << "Bad result2 " << result2 << " was expecting 165." << std::endl;
		return false;
	}

	const auto input3 = ParseInput(R"(157 ORE => 5 NZVS
165 ORE => 6 DCFZ
44 XJWVT, 5 KHKGT, 1 QDVJ, 29 NZVS, 9 GPVTF, 48 HKGWZ => 1 FUEL
12 HKGWZ, 1 GPVTF, 8 PSHF => 9 QDVJ
179 ORE => 7 PSHF
177 ORE => 5 HKGWZ
7 DCFZ, 7 PSHF => 2 XJWVT
165 ORE => 2 GPVTF
3 DCFZ, 7 NZVS, 5 HKGWZ, 10 PSHF => 8 KHKGT)");
		const auto result3 = OreRequirement({"FUEL", 1}, input3);
	if(result3 != 13312){
		std::cerr << "Bad result3 " << result3 << " was expecting 13312." << std::endl;
		return false;
	}
	const auto fuel3 = FuelFromTrillionOre(input3);
	if(fuel3 != 82892753){
		std::cerr << "Bad fuel3 " << fuel3 << " was expecting 82892753." << std::endl;
		return false;
	}

	const auto input4 = ParseInput(R"(2 VPVL, 7 FWMGM, 2 CXFTF, 11 MNCFX => 1 STKFG
17 NVRVD, 3 JNWZP => 8 VPVL
53 STKFG, 6 MNCFX, 46 VJHF, 81 HVMC, 68 CXFTF, 25 GNMV => 1 FUEL
22 VJHF, 37 MNCFX => 5 FWMGM
139 ORE => 4 NVRVD
144 ORE => 7 JNWZP
5 MNCFX, 7 RFSQX, 2 FWMGM, 2 VPVL, 19 CXFTF => 3 HVMC
5 VJHF, 7 MNCFX, 9 VPVL, 37 CXFTF => 6 GNMV
145 ORE => 6 MNCFX
1 NVRVD => 8 CXFTF
1 VJHF, 6 MNCFX => 4 RFSQX
176 ORE => 6 VJHF)");
	const auto result4 = OreRequirement({"FUEL", 1}, input4);
	if (result4 != 180697)
	{
		std::cerr << "Bad result4 " << result4 << " was expecting 180697." << std::endl;
		return false;
	}
	const auto fuel4 = FuelFromTrillionOre(input4);
	if(fuel4 != 5586022){
		std::cerr << "Bad fuel4 " << fuel4 << " was expecting 5586022." << std::endl;
		return false;
	}

	const auto input5 = ParseInput(R"(171 ORE => 8 CNZTR
7 ZLQW, 3 BMBT, 9 XCVML, 26 XMNCP, 1 WPTQ, 2 MZWV, 1 RJRHP => 4 PLWSL
114 ORE => 4 BHXH
14 VRPVC => 6 BMBT
6 BHXH, 18 KTJDG, 12 WPTQ, 7 PLWSL, 31 FHTLT, 37 ZDVW => 1 FUEL
6 WPTQ, 2 BMBT, 8 ZLQW, 18 KTJDG, 1 XMNCP, 6 MZWV, 1 RJRHP => 6 FHTLT
15 XDBXC, 2 LTCX, 1 VRPVC => 6 ZLQW
13 WPTQ, 10 LTCX, 3 RJRHP, 14 XMNCP, 2 MZWV, 1 ZLQW => 1 ZDVW
5 BMBT => 4 WPTQ
189 ORE => 9 KTJDG
1 MZWV, 17 XDBXC, 3 XCVML => 2 XMNCP
12 VRPVC, 27 CNZTR => 2 XDBXC
15 KTJDG, 12 BHXH => 5 XCVML
3 BHXH, 2 VRPVC => 7 MZWV
121 ORE => 7 VRPVC
7 XCVML => 6 RJRHP
5 BHXH, 4 VRPVC => 5 LTCX)");
	const auto result5 = OreRequirement({"FUEL", 1}, input5);
	if (result5 != 2210736) {
		std::cerr << "Bad result5 " << result5 << " was expecting 2210736." << std::endl;
		return false;
	}
	const auto fuel5 = FuelFromTrillionOre(input5);
	if(fuel5 != 460664){
		std::cerr << "Bad fuel5 " << fuel5 << " was expecting 460664." << std::endl;
		return false;
	}

	return true;
}

int main(int, const char **)
{
	std::cout << "Day 14" << std::endl;

	if(!Test()){
		std::cerr << "Tests failed." << std::endl;
		return EXIT_SUCCESS;
	}

	const std::string input{
		std::istreambuf_iterator<char>(std::cin),
		std::istreambuf_iterator<char>()};

	const auto reactions = ParseInput(input);

	std::cout << "First " << OreRequirement({"FUEL", 1}, reactions) << std::endl;

	std::cout << "Second " << FuelFromTrillionOre(reactions) << std::endl;

	return EXIT_SUCCESS;
}
