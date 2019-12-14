#ifndef IntCodeComputer_H
#define IntCodeComputer_H

#include <ostream>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <sstream>
#include <map>

template <typename Tint>
class IntCodeComputer
{
	std::map<Tint, Tint> data;
	std::function<void(const Tint)> outputHandler = nullptr;
	std::function<Tint(void)> inputHandler = nullptr;
	bool dasm = 0;

public:
	enum Subsystem {
		AirCondition = 1,
		ThermalRadiatorController = 5
	};

	void SetInputHandler(const decltype(inputHandler) &handler) {
		this->inputHandler = handler;
	}

	void ResetInputHandler() {
		SetInputHandler([]() -> Tint {
			throw std::runtime_error("No input handler is registered.");
		});
	}

	void SetOutputHandler(const decltype(outputHandler) &handler) {
		this->outputHandler = handler;
	}

	void ResetOutputHandler() {
		SetOutputHandler([](const Tint value) -> void {
			throw std::runtime_error(std::string("No output handler is registered to handle the value ") + std::to_string(value));
		});
	}

	IntCodeComputer(const std::initializer_list<Tint> &&il)
		: data(il)
	{
		InitHandlers();
	}

	IntCodeComputer(const IntCodeComputer &other)
		: data{ other.data }
	{
		InitHandlers();
	}

	IntCodeComputer(std::istream &stream)
	{
		InitHandlers();
		ReadStream(stream);
	}

	IntCodeComputer(const std::string &input)
	{
		InitHandlers();

		std::istringstream ss{ input };
		ReadStream(ss);
	}

	void InitHandlers() {
		ResetInputHandler();
		ResetOutputHandler();
	}

	void ReadStream(std::istream &stream) {
		// Read the program from the input
		if (!stream) {
			throw std::runtime_error("Bad stream");
		}

		Tint pos = 0;
		Tint tmp;
		stream >> tmp;
		data[pos++] = tmp;

		while (stream && !stream.eof()) {
			// consume the ,
			stream.get();

			if (!stream) {
				throw std::runtime_error("Bad stream after consuming a \',\'.");
			}
			//read the number
			stream >> tmp;
			data[pos++] = tmp;
		}
	}

	auto &operator[](const size_t position) const {
		return data.at(position);
	}

	auto &operator[](const size_t position) {
		if (data.find(position) == data.end())
		{
			data[position] = 0;
		}
		return data.at(position);
	}

	enum Mode {
		Position = 0,
		Immediatte,
		Relative
	};

	Tint RelativeBaseOffset = 0;

	bool Execute() {
		for (Tint pc = 0; ;++pc) {

			const auto &opcode = (*this)[pc];

			auto Param = [&](Tint i) -> Tint& {
				static const Tint modeMask[3] = { 100, 1000, 10000 };
				Tint pMode = (opcode / modeMask[i - 1]) % 10;

				auto& memVal = (*this)[pc + i];
				switch (Mode(pMode)) {
				case Mode::Immediatte:
					if (dasm)std::cout << " " << memVal;
					return memVal;

				case Mode::Position:
					if (dasm) std::cout << " [" << memVal << "]";
					return (*this)[memVal];

				case Mode::Relative:
					if (dasm) std::cout << " [" << memVal << " (rel " << RelativeBaseOffset << ")]";
					return (*this)[memVal + RelativeBaseOffset];

				default:
					throw std::runtime_error("Unsupported mode " + std::to_string(pMode));
				}
			};

			if (dasm) std::cout << pc << ":\t";

			switch (opcode % 100) {
			case 99:
				if (dasm) std::cout << "halt\t" << std::endl;
				return true;

			case 1:
				if (dasm) std::cout << "add\t";
				Param(3) = Param(1) + Param(2);
				pc += 3;
				break;

			case 2:
				if (dasm) std::cout << "mul\t";
				Param(3) = Param(1) * Param(2);
				pc += 3;
				break;

			case 3: {
				if (dasm) std::cout << "in\t";
				auto& p = Param(1);
				p = inputHandler();
				if (dasm) std::cout << " < " << p;
				++pc;
				break;
			}
			case 4:
				if (dasm) std::cout << "out\t";
				outputHandler(Param(1));
				++pc;
				break;

			case 5: {
				if (dasm) std::cout << "jt\t";
				const auto p1 = Param(1);
				const auto p2 = Param(2);
				if (p1) {
					pc = p2 - 1;
				}
				else {
					pc += 2;
				}
				break;
			}
			case 6: {
				if (dasm) std::cout << "jf\t";
				const auto p1 = Param(1);
				const auto p2 = Param(2);
				if (p1 == 0) {
					pc = p2 - 1;
				}
				else {
					pc += 2;
				}
				break;
			}

			case 7:
				if (dasm) std::cout << "lt\t";
				Param(3) = (Param(1) < Param(2));
				pc += 3;
				break;

			case 8:
				if (dasm) std::cout << "eq\t";
				Param(3) = (Param(1) == Param(2));
				pc += 3;
				break;

			case 9:
				if (dasm) std::cout << "rel\t";
				RelativeBaseOffset += Param(1);
				pc += 1;
				break;

			default:
				std::cerr << "Bad Opcode [" << (opcode % 100) << "] at position " << pc << std::endl;
				return false;
			}
			if (dasm) std::cout << std::endl;
		}

		return true;
	}
};

#endif
