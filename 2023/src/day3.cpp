#include "utils.h"
#include <ranges>
#include <tuple>
#include <iostream>
#include <numeric>
#include <optional>
#include <map>

using namespace std;

const string test_raw_input = R"(467..114..
...*......
..35..633.
......#...
617*......
.....+.58.
..592.....
......755.
...$.*....
.664.598..)";

struct Point{
    int x;
    int y;

    constexpr bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

    constexpr Point operator+(const Point& other) const {
        return Point {
            x + other.x,
            y + other.y
        };
    }

    constexpr Point operator-(const Point& other) const {
        return Point{
            x - other.x,
            y - other.y
        };
    }

    constexpr Point operator*(const int& value) const {
        return Point{
            x * value,
            y * value
        };
    }

    constexpr Point operator/(const int& value) const {
        return Point{
            x / value,
            y / value
        };
    }

    constexpr Point right() const {
        return Point{
            x + 1,
            y
        };
    }

    constexpr Point left() const {
        return Point{
            x - 1,
            y
        };
    }

    constexpr Point up() const {
        return Point{
            x,
            y - 1
        };
    }

    constexpr Point down() const {
        return Point{
            x,
            y + 1
        };
    }
};

ostream& operator<<(ostream& os, const Point& p) {
    return os << "(" << p.x << ", " << p.y << ")";
}

struct NumberView{
    Point base;
    size_t length;
};
ostream& operator<<(ostream& os, const NumberView& nv) {
    return os << nv.base << "_" << nv.length;
}

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

ostream& operator << (ostream& os, const BoundingBox& bb) {
    return os << "BoundningBox{" << bb.topLeft << ", " << bb.bottomRight << "}";
}


vector<NumberView> extractNumbers(int y, string line) {
    bool inNumber = false;
    vector<NumberView> result;

    for(size_t i = 0; const char& ch : line) {
        if(isdigit(ch)) {
            if(inNumber) {
                // extend current number
                result.back().length++;
            }
            else{
                // add a number
                result.emplace_back(Point{static_cast<int>(i), y}, 1);
                inNumber = true;
            }
        }
        else{
            inNumber = false;
        }
        i++;
    }

    return result;
}


long first(const vector<string>& input) {
    BoundingBox mapLimits{
        {0, 0}, {static_cast<int>(input[0].size()-1), static_cast<int>(input.size()-1)}
    };

    auto tmpRes = views::iota(0UL, input.size())
        | views::transform([&input](const auto& y) {
            return make_tuple(y, input[y]);
        })
        | views::transform([](const auto& entry) {
            const auto& [index, line] = entry;
            return extractNumbers(index, line);
        })
        | views::join
        | views::filter([&input, &mapLimits](const NumberView& entry) {
            const auto bb = BoundingBox{
                entry.base.up().left(),
                entry.base.down() + Point{static_cast<int>(entry.length), 0}
            }.restrictIn(mapLimits);

            bool hasSymbol = false;
            bb.for_each_cell([&hasSymbol, &input](const Point& p) {
                const auto ch = input.at(p.y).at(p.x);
                if(ch != '.' && !isdigit(ch)){
                    hasSymbol = true;
                }
            });

            return hasSymbol;
        })
        | views::transform([&input](const NumberView& entry) {
            // extract number
            const auto line = input.at(entry.base.y);

            long number = 0;
            for(const char& ch: line.substr(entry.base.x, entry.length)){
                if(!isdigit(ch)) {
                    throw std::runtime_error("Not a digit");
                }
                // assume ascii representation of char with increasing values starting at '0' 
                number = number * 10 + ch -'0';
            }
            return number;
        })
        | views::common
        ;

    // for(size_t i = 0; const auto element: tmpRes) {
    //     cout << i++ << ":\t" << element << '\n';
    // }

    const auto result = accumulate(tmpRes.begin(), tmpRes.end(), 0);
    return result;
}

long second(vector<string> input) {
    BoundingBox mapLimits{
        {0, 0}, {static_cast<int>(input[0].size()-1), static_cast<int>(input.size()-1)}
    };

    auto tmpRes = views::iota(0UL, input.size())
        | views::transform([&input](const auto& y) {
            return make_tuple(y, input[y]);
        })
        | views::transform([](const auto& entry) {
            const auto& [index, line] = entry;
            return extractNumbers(index, line);
        })
        | views::join
        | views::transform([&input, &mapLimits](const NumberView& entry) {
            const auto bb = BoundingBox{
                entry.base.up().left(),
                entry.base.down() + Point{static_cast<int>(entry.length), 0}
            }.restrictIn(mapLimits);

            optional<Point> gearLocation{};

            bb.for_each_cell([&gearLocation, &input](const Point& p) {
                const auto ch = input.at(p.y).at(p.x);
                if(ch == '*'){
                    // cout << "found gear symbol '" << ch << "' at " << p << endl;
                    if(gearLocation.has_value()){
                        throw runtime_error("Number has already a gear on int's bounding box");
                    }
                    gearLocation.emplace(p);
                }
            });

            // cout << "Gear location " << gearLocation << endl;

            return make_pair(entry, gearLocation);
        })
        | views::filter([](const auto& p){
            return p.second.has_value();
        })
        | views::transform([&input](const auto& p) {
            auto& [entry, optGearLocation] = p;
            // extract number
            const auto line = input.at(entry.base.y);

            long number = 0;
            for(const char& ch: line.substr(entry.base.x, entry.length)){
                if(!isdigit(ch)) {
                    throw std::runtime_error("Not a digit");
                }
                // assume ascii representation of char with increasing values starting at '0' 
                number = number * 10 + ch -'0';
            }

            return make_pair(number, optGearLocation.value());
        });

    auto pointKeyComparator = [](const Point& lhs, const Point& rhs) {
        return lhs.x == rhs.x ? lhs.y < rhs.y : lhs.x < rhs.x;
    };

    // group by key location
    map<Point, vector<long>, decltype(pointKeyComparator)> gearAssociation;

    for(const auto& [number, location]: tmpRes) {
        if(gearAssociation.find(location) == gearAssociation.end()){
            gearAssociation[location] = vector<long>{{number}};
        }
        else{
            gearAssociation[location].emplace_back(number);
        }
    }

    auto tmpResult2 = gearAssociation
        | views::filter([](const auto& entry){
            auto& [key, values] = entry;

            // cout << key << ": " << values << endl;
            if(values.size() == 1) {
                // cout << "filtering " << values << endl;
                return false;
            }
            if(values.size() > 2) {
                throw runtime_error("More that two numbers next to a gear");
            }

            return true;
        })
        | views::transform([](const auto& entry) {
            auto& [key, values] = entry;
            return values.at(0) * values.at(1);
        });

    // for(size_t i = 0; const auto& element: tmpResult2) {
    //     cout << i++ << ":\t" << element << '\n';
    // }

    const auto result = accumulate(tmpResult2.begin(), tmpResult2.end(), 0);

    return result;
}

int main(const int, const char* []) {
    cout << "Day 3" << endl;

    const auto first_test_result = first(utils::split(test_raw_input));
    assertEquals(4361, first_test_result);
    cout << "Test ok" << endl;

    const auto input = utils::readFile("input/day3");
    const auto first_result = first(input);
    cout << "First: " << first_result << endl;

    const auto second_test_result = second(utils::split(test_raw_input));
    assertEquals(467835, second_test_result);
    cout << "Test 2 ok" << endl;

    const auto second_result = second(input);
    cout << "Second: " << second_result << endl;

    return EXIT_SUCCESS;
}