#ifndef Memory_H
#define Memory_H

#include <vector>
#include <ostream>
#include <optional>
#include <iostream>

class Memory
{
	std::vector<int> data;

public:
	Memory(const std::initializer_list<int> &&il)
		: data(il)
	{
	}

	Memory(const std::vector<int> &&input, size_t extendTo = 0)
		: data(input)
	{
		if (data.size() < extendTo){
			data.resize(extendTo);
		}
	}

	Memory(const Memory &other)
		: data(other.data)
	{
	}

	int operator[](const size_t position) const {
		return data.at(position);
	}

	int &operator[](const size_t position) {
		return data.at(position);
	}

	std::vector<int>::size_type size() const {
		return data.size();
	}

	bool Execute() {
		for (std::vector<int>::size_type pc = 0; (*this)[pc] != 99 ; pc += 4) {

			const auto &opcode = (*this)[pc];

			int (*op)(int v1, int v4);

			switch (opcode) {
			case 1:
				op = [](const int v1, const int v2) -> int {return v1 + v2; };
				break;

			case 2:
				op = [](const int v1, const int v2) -> int { return v1 * v2; };
				break;

			default:
				std::cerr << "Bad Opcode [" << opcode << "] at position " << pc << std::endl;
				return false;
			}

			const int p1 = (*this)[pc + 1];
			const int p2 = (*this)[pc + 2];
			const int to = (*this)[pc + 3];

			const int v1 = (*this)[p1];
			const int v2 = (*this)[p2];

			(*this)[to] = op(v1, v2);
		}

		return true;
	}

	static std::optional<Memory>  ReadFromInput() {
		// Read the program from the input
		std::vector<int> data;

		int tmp;
		if (!std::cin){
			return std::nullopt;
		}
		std::cin >> tmp;
		data.push_back(tmp);

		while(std::cin && !std::cin.eof()){
			// consume the ,
			std::cin.get();

			if (!std::cin){
				return std::nullopt;
			}
			//read the number
			std::cin >> tmp;
			data.push_back(tmp);
		}

		return std::make_optional<Memory>(std::move(data));
	}

	friend std::ostream &operator<<(std::ostream &os, const Memory &m);
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
	os << "{";
	if (v.size() != 0) {
		os << v[0];
	}

	for (auto i = std::cbegin(v) + 1; i != std::cend(v); ++i) {
		os << ", " << *i;
	}

	os << "}";

	return os;
}

std::ostream &operator<<(std::ostream &os, const Memory &m) {
	os << m.data;

	return os;
}

#endif
