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

typedef std::vector<int> Layer; // should be vector_view
typedef std::vector<int> Image;

int main(int, char**){
	std::cout << "Day 8" << std::endl;

	std::vector<int> input;
	std::transform(
		std::istream_iterator<char>(std::cin), std::istream_iterator<char>(),
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
			input.begin() + (i * d.size()),
			input.begin() + ((i+1) * d.size()));
	}

	std::map<std::pair<const Layer * const, const int>, const int> countMemory;
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

	const auto &selected = std::min_element(layers.cbegin(), layers.cend(), [&count](const auto &l1, const auto &l2) {
		return count(l1, 0) < count(l2, 0);
	});

	const auto result = count(*selected, 1) * count(*selected, 2);

	std::cout << "Result: " << result << std::endl;

	// Render Image
	enum Color {
		Black = 0,
		White,
		Transparent
	};

	const auto image = std::accumulate(
		layers.crbegin(), layers.crend(),
		Image(layers.front().size(), Color::Black),
		[](auto &image, const auto &layer) {
			// apply layer onto image
			std::transform(
				image.begin(), image.end(),
				layer.begin(),
				image.begin(),
				[](const auto &imageValue, const auto &layerValue){
					return layerValue == Color::Transparent ? imageValue : layerValue;
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
