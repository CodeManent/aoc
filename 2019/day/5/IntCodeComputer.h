#ifndef IntCodeComputer_H
#define IntCodeComputer_H

#include <vector>
#include <ostream>
#include <iostream>
#include <stdexcept>

template<typename T>
std::ostream& operator<< (std::ostream & os, const std::vector<T>& v){
	os << '{';
	auto i = v.begin();
	if (i != v.end()) {
		os << *i;
	}

	while(++i != v.end()){
		os << ", " << *i;
	}
	os << '}';

	return os;
}

class IntCodeComputer
{
	std::vector<int> data;
	void (*outputHandler)(const int value) = nullptr;
	int (*inputHandler)() = nullptr;
	bool dasm = 0;

public:
	enum Subsystem{
		AirCondition = 1,
		ThermalRadiatorController = 5
	};

	void SetInputHandler(int (handler)()){
		this->inputHandler = handler;
	}

	void ResetInputHandler() {
		SetInputHandler([]() -> int {
			throw std::runtime_error("No input handler is registered.");
		});
	}

	void SetOutputHandler(void (handler)(const int value)){
		this->outputHandler = handler;
	}

	void ResetOutputHandler(){
		SetOutputHandler([](const int value){
			throw std::runtime_error(std::string("No output handler is registered to handle the value ") + std::to_string(value));
		});
	}
	
	IntCodeComputer(const std::initializer_list<int> &&il)
		: data(il)
	{
		ResetInputHandler();
		ResetOutputHandler();
	}

	IntCodeComputer(const std::vector<int> &&input, size_t extendTo = 0)
		: data(input)
	{
		if (data.size() < extendTo){
			data.resize(extendTo);
		}
		ResetInputHandler();
		ResetOutputHandler();
	}

	IntCodeComputer(const IntCodeComputer &other)
		: data(other.data)
	{
		ResetInputHandler();
		ResetOutputHandler();
	}

	IntCodeComputer(std::istream &stream)
	{
		ResetInputHandler();
		ResetOutputHandler();

		// Read the program from the input
		if (!stream){
			throw std::runtime_error("Bad stream");
		}

		int tmp;
		stream >> tmp;
		data.push_back(tmp);

		while(stream && !stream.eof()){
			// consume the ,
			stream.get();

			if (!stream){
				throw std::runtime_error("Bad stream after consuming a \',\'.");
			}
			//read the number
			stream >> tmp;
			data.push_back(tmp);
		}
		data.resize(99999+1);
	}

	auto operator[](const size_t position) const {
		return data.at(position);
	}

	auto &operator[](const size_t position) {
		return data.at(position);
	}

	std::vector<int>::size_type size() const {
		return data.size();
	}

	enum Mode{
		Position = 0,
		Immediatte,
		IO
	};

	bool Execute() {
		for (std::vector<int>::size_type pc = 0; ;++pc) {

			const auto &opcode = (*this)[pc];

			auto Param = [&](int i) -> int& {
				static const int modeMask[3] = {100, 1000, 10000};
				int pMode = (opcode / modeMask[i-1]) % 10;

				auto& memVal = (*this)[pc + i];
				if(!pMode){
					if (dasm) std::cout << " ["<< memVal << "-" << (*this)[memVal] << "]";
					return (*this)[memVal];
				}
				if (dasm) std::cout << " " << memVal;
				return memVal;
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

			case 3:{
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

			case 5:{
				if (dasm) std::cout << "jt\t";
				const auto p1 = Param(1);
				const auto p2 = Param(2);
				if(p1){
					pc = p2 - 1;
				}
				else{
					pc += 2;
				}
				break;
			}
			case 6:{
				if (dasm) std::cout << "jf\t";
				const auto p1 = Param(1);
				const auto p2 = Param(2);
				if(p1 == 0){
					pc = p2 - 1;
				}
				else{
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

			default:
				std::cerr << "Bad Opcode [" << (opcode % 100) << "] at position " << pc << std::endl;
				return false;
			}
			if (dasm) std::cout << std::endl;
		}

		return true;
	}
	
	int ReadParameter(int address){
		return 0;
	}

	std::ostream &operator<<(std::ostream &os) const {
		os << data;
		return os;
	}
};

#endif
