#ifndef AOC_UTILS_h
#define AOC_UTILS_h

#include "Point.h"

#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <utility> // pair
#include <iterator>
#include <algorithm>
#include <optional>
#include <iterator>

#include <cstdlib>

namespace utils {
    using namespace std;

    vector<string> readFile(string path) {
        vector<string> data;
        ifstream infile(path);
        if (!infile) {
            throw runtime_error("Couldn't open " + path);
        }

        string str;
        while (getline(infile, str)) {
            data.push_back(str);
        }

        return data;
    }

    vector<string> split(const string& str, const char& delimeter = '\n')
    {
        auto result = vector<string>{};
        auto ss = stringstream{str};

        for (string line; getline(ss, line, delimeter);)
            result.push_back(line);

        return result;
    }

    template <typename T>
    struct Grid {
        vector<vector<T>> data;

        const T& at(const Point&& p) const {
            return data.at(p.y).at(p.x);
        }
        T& at(const Point&& p) {
            return data.at(p.y).at(p.x);
        }
        
        const vector<T>& at(const size_t&& p) const {
            return data.at(p);
        }
        vector<T>& at(const size_t&& p) {
            return data.at(p);
        }
    };

    struct BoundingBox{
        Point topLeft;
        Point bottomRight;

        BoundingBox restrictIn(const BoundingBox& other) const {
            return BoundingBox{
                Point{
                    max(topLeft.x, other.topLeft.x),
                    max(topLeft.y, other.topLeft.y)
                },
                Point{
                    min(bottomRight.x, other.bottomRight.x),
                    min(bottomRight.y, other.bottomRight.y)
                }
            };
        }

        template <typename T>
        void for_each_cell(T callable) const {
            for(Point lineStart = topLeft; lineStart.y <= bottomRight.y; lineStart = lineStart.down()) {
                for(Point current = lineStart; current.x <= bottomRight.x; current = current.right()) {
                    callable(current);
                }
            }
        }
    };

    struct NumberView{
        Point base;
        size_t length;
    };

    template <typename T>
    struct InputStreamMatcher{
        T value;
    };

    template <typename TValue = unsigned long, typename TLength = size_t>
    struct GeneralExtent{
        TValue base;
        TLength length;

        using CurrentExtentType = GeneralExtent<TValue, TLength>;

        constexpr TValue end() const {
            return base + length;
        }

        constexpr bool hit(TValue number) const {
            return (base <= number) && (number <= (base + length-1));
        }

        constexpr std::pair<std::optional<CurrentExtentType>, std::optional<CurrentExtentType>> split(TValue number) const {
            if(number <= base) {
                return std::make_pair(nullopt, std::make_optional(*this));
            }
            if(end() < number) {
                return std::make_pair(std::make_optional(*this), nullopt);
            }
            const auto d = number - base;
            return std::make_pair(
                std::make_optional(CurrentExtentType{base, d}),
                std::make_optional(CurrentExtentType{number, length - d})
            );
        }
    };

    using Extent = GeneralExtent<>;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const utils::Grid<T>& grid);

void assertEquals(const auto& expected, const auto& actual, const std::optional<std::string> message = std::nullopt) {
    if(expected != actual) {
        std::cerr << "Expected: " << expected << "\n" << "Actual: " << actual << std::endl;
        if(message.has_value() && message.value().size() != 0) {
            throw std::runtime_error(message.value());
        }
        else{
            throw std::runtime_error("Expected value does not equal to the actual one");
        }
    }
}

void assertNotEquals(const auto& unexpected, const auto& actual, const std::optional<std::string> message = std::nullopt) {
    if(unexpected == actual) {
        std::cerr << "Unexpected value: " << actual << std::endl;
        if(message.has_value() && message.value().size() != 0) {
            throw std::runtime_error(message.value());
        }
        else{
            throw std::runtime_error("Unexpected value equals the actual one");
        }
    }
}

template <typename T> 
std::ostream& operator<<(std::ostream& os, const std::vector<T>& input){
    os << "vector[";
    bool first = true;
    for(const auto& elem : input) {
        if(!first) {
            os << ", ";
        }
        os << elem;
        first = false;
    }
    os << "]";
    return os;
}

template <typename T>
void print(const std::vector<T>& input, const std::string& name = "vector") {
    using namespace std;

    size_t counter = 0;
    cout << name << "[\n";

    for(const auto &element: input){
        cout << counter++ << ":\t" <<  element << '\n';
    }
    cout << "]" << endl;
}

template<typename T1, typename T2>
std::ostream& operator<< (std::ostream& os, const std::pair<T1, T2>& p){
    return os << "pair[" << p.first << ", " << p.second << "]";
}

// Fill the vector from the input stream
template <typename T>
std::istream& operator >> (std::istream& is, std::vector<T>& v) {
    std::copy(
        std::istream_iterator<T>(is),
        std::istream_iterator<T>(),
        std::back_inserter(v));
    return is;
}

template <typename T>
constexpr auto is_equal_to(const T& value){
    return [&value](const T& toCheck){
        return value == toCheck;
    };
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const utils::Grid<T>& grid){
    for(const auto& line: grid.data){
        for(const auto& ch: line) {
            os << ch;
        }
        os << '\n';
    }
    return os;
}

std::ostream& operator << (std::ostream& os, const utils::BoundingBox& bb) {
    return os << "BoundningBox{" << bb.topLeft << ", " << bb.bottomRight << "}";
}

std::ostream& operator<<(std::ostream& os, const utils::NumberView& nv) {
    return os << nv.base << "_" << nv.length;
}

template <typename TValue, typename TLength>
std::ostream& operator<< (std::ostream& os, const utils::GeneralExtent<TValue, TLength>& extent) {
    return os << "Extent[" << extent.base << ", " << extent.length << "]";
}

template <typename T>
utils::InputStreamMatcher<T> matchConsume(T value) {
    return utils::InputStreamMatcher{value};
}

std::istream& operator>>(std::istream& is, utils::InputStreamMatcher<char> toMatch) {
    char actual = is.get();
    assertEquals(toMatch.value, actual);
    return is;
}

std::istream& operator>>(std::istream& is, utils::InputStreamMatcher<const char*> toMatch) {
    size_t length = std::char_traits<const char>::length(toMatch.value);
    std::vector<char> buffer;
    buffer.reserve(length);
    
    for(size_t i = 0; i < length ; ++i) {
        buffer.emplace_back(is.get());
    }

    auto [left, right] = mismatch(toMatch.value, toMatch.value + length, begin(buffer), end(buffer));
    if(left != toMatch.value + length){
        std::cerr << "Expected: " << toMatch.value;
        std::cerr << "\nActual:" << std::string{buffer.begin(), buffer.end()};
        std::cerr << "\nDifference at position: " << std::distance(toMatch.value, left);
        std::cerr << std::endl;

        throw std::runtime_error("Expected value does not equal to the actual one");
    }
    return is;
}

std::istream& operator>>(std::istream& is, utils::InputStreamMatcher<const std::string> toMatch) {
    std::stringstream ss;
    for(size_t i = 0; i < toMatch.value.length(); ++i) {
        ss.put(is.get());
    }

    assertEquals(toMatch.value, ss.str());
    return is;
}

#endif
