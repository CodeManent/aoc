#include "day.h"
#include "utils.h"

#include <iostream>
#include <functional>
#include <optional>
#include <numeric>
#include <sstream>
#include "Range.h"

using namespace std;

class CPU {
public:
	size_t cycle = 0;
	long X = 1;

	size_t targetCycle = 20;
	optional<function<void(const CPU&)>> cycleReporter = nullopt;
	optional<function<void(const CPU&)>> signalReporter = nullopt;

	void execute(const string& command) {
		string opcode;

		tick();
		if (command == "noop") {
			// no-op
		}
		else if (command.starts_with("addx ")) {
			int param = atoi(command.data() + 5);
			tick();

			X += param;
		}
	}

	long signalStrength() const {
		return static_cast<long>(cycle) * X;
	}

	void tick() {
		++cycle;

		if (cycleReporter.has_value()) {
			cycleReporter.value()(*this);
		}

		if (targetCycle == cycle) {
			handleTargetCycleReached();
		}
	}

	void registerCycleReporter(function<void(const CPU&)> reporter) {
		cycleReporter = reporter;
	}
	void clearCycleReporter() {
		cycleReporter = nullopt;
	}

	void registerSignalReporter(function<void(const CPU&)> reporter) {
		signalReporter = reporter;
	}
	void clearSignalReporter() {
		signalReporter = nullopt;
	}

	void handleTargetCycleReached() {
		targetCycle += 40;

		if (signalReporter.has_value()) {
			// const auto ss = signalStrength();
			signalReporter.value()(*this);
		}

	}
};

class CRT {
public:
	static constexpr const size_t lines = 6;
	static constexpr const size_t columns = 40;

	vector<char> framebuffer;

	CRT()
		:framebuffer(lines * columns, ' ')
	{
	}

	void samplePixel(const CPU& cpu) {
		const auto fbIndex = cpu.cycle - 1;
		const auto linePixelPos = fbIndex % columns;

		bool isLit = Range(cpu.X - 1, cpu.X + 1).contains(linePixelPos);

 		if (isLit) {
			framebuffer.at(fbIndex) = '#';
		}
	}

	string frame() {
		string frame;
		frame.reserve(lines * columns);

		vector<char>::const_iterator fbIter = framebuffer.begin();
		for (size_t line = 0; line < lines; ++line) {
			for (size_t column = 0; column < columns; ++column) {
				frame.push_back(*fbIter++);
			}
			frame.push_back('\n');
		}

		return frame;
	}
};

#include <algorithm>
#include <ranges>

DayResult day10() {
	const auto lines = readFile("day_10_input.txt");
	CPU cpu;
	CRT crt;

	vector<long> signalHistory{};

	// register hooks
	cpu.registerSignalReporter([&signalHistory](const CPU& lambdaCPU) {
		signalHistory.push_back(lambdaCPU.signalStrength());
	});

	cpu.registerCycleReporter([&crt](const auto& lambdaCPU) {
		crt.samplePixel(lambdaCPU);
	});


	// execute commands
	ranges::for_each(lines, [&cpu](const auto line) {
		cpu.execute(line);
	});

	// sample results
	size_t signal6Sum = accumulate(signalHistory.cbegin(), signalHistory.cbegin() + 6, 0l);

	return {
		make_optional<PartialDayResult>({"Signal strength sum", to_string(signal6Sum)}),
		make_optional<PartialDayResult>({"CRT frame", "\n" + crt.frame()}),
	};
}
