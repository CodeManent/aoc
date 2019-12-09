#include <iostream>
#include <cstdlib>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <map>

struct Dimmentions{
	int width;
	int height;
	constexpr int size() const {
		return width * height;
	}
};

typedef std::vector<int> Layer;
typedef std::vector<int> Image;

enum Color {
	Black = 0,
	White,
	Transparent
};

int main(int, char**){
	std::cout << "Day 8" << std::endl;

	std::vector<int> input;
	std::transform(
		std::istream_iterator<char>(std::cin),
		std::istream_iterator<char>(),
		std::back_inserter(input),
		[](const char ch){
			return int(ch) - '0';
		});

	constexpr const Dimmentions d{25, 6};
	const auto layerCount = input.size() / d.size();
	std::vector<Layer> layers;
	layers.reserve(layerCount);

	for(auto i = decltype(layerCount){0}; i < layerCount; ++i) {
		layers.emplace_back(
			input.cbegin() + (i * d.size()),
			input.cbegin() + ((i+1) * d.size()));
	}

	std::map<std::pair<const Layer *, int>, int> countMemory;
	const auto count = [&](const auto &v, const int elem) -> int {
		const auto key = std::make_pair(&v, elem);

		const auto existing = countMemory.find(key);
		if(existing != countMemory.cend()){
			return existing->second;
		}

		const auto result = std::count(v.cbegin(), v.cend(), elem);
		countMemory.insert({key, result});

		return result;
	};

	const auto &selected = std::min_element(
		layers.cbegin(),
		layers.cend(),
		[&count](const auto &l1, const auto &l2) {
			return count(l1, 0) < count(l2, 0);
		});

	const auto result = count(*selected, 1) * count(*selected, 2);

	std::cout << "Result: " << result << std::endl;

	// Render Image

	const auto image = std::accumulate(
		layers.crbegin(),
		layers.crend(),
		Image(d.size(), Color::Black),
		[](auto &image, const auto &layer) {
			// apply layer onto image
			std::transform(
				image.begin(), image.end(),
				layer.begin(),
				image.begin(),
				[](const auto &imgValue, const auto &lValue){
					return lValue == Color::Transparent ? imgValue : lValue;
				});
			return image;
		});

	// display image
	for(int y = 0; y < d.height; ++y) {
		for(int x = 0; x < d.width; ++x){
			switch(image[y*d.width + x]){
				case Color::Black:       std::cout << ' '; break;
				case Color::White:       std::cout << '.'; break;
				case Color::Transparent: std::cout << ' '; break;
			}
		}
		std::cout << std::endl;
	}

	return EXIT_SUCCESS;
}
