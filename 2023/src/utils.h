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
#include <ranges>
#include <charconv>

#include <cstdlib>

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
        using value_type = T;

        vector<vector<T>> data;

        void grow(size_t width, size_t height, T fill) {
            for(size_t row = 0; row < height; ++row) {
                if(row < data.size()){
                    // expand existing line
                    vector<T>& dataRow = data[row];
                    dataRow.reserve(max(width, dataRow.size()));
                    for(size_t column = dataRow.size(); column < width; ++column) {
                        dataRow.emplace_back(fill);
                    }
                }
                else{
                    data.emplace_back(vector<T>(width, fill));
                }
            }
        }


        template <typename TPointElement>
        const T& at(const GenericPoint<TPointElement>& p) const {
            return data.at(p.y).at(p.x);
        }

        template <typename TPointElement>
        T& at(const GenericPoint<TPointElement>& p) {
            return data.at(p.y).at(p.x);
        }

        /* Row element access pattern */
        template <typename GridT>
        struct RowElementIterator {
            using iterator_category = std::contiguous_iterator_tag;
            using value_type = GridT::value_type;
            using difference_type = std::ptrdiff_t;
            using pointer = value_type*;
            using reference = value_type&;

            GridT * grid;
            GenericPoint<size_t> p;
            explicit RowElementIterator(): RowElementIterator{nullptr, 0} {}
            explicit RowElementIterator(GridT * const grid, const size_t column, const size_t row)
                : grid{grid}
                , p{column, row}
            {
            }
            constexpr RowElementIterator(const RowElementIterator&) = default;
            constexpr RowElementIterator& operator=(const RowElementIterator& other) {
                grid = other.grid;
                p = other.p;
                return *this;
            }
            constexpr RowElementIterator& operator=(RowElementIterator&& other) {
                grid = other.grid;
                p = other.p;
                return *this;
            }
            RowElementIterator& operator++() { p.x++; return *this; }
            RowElementIterator operator++(int) {auto retval = *this; ++(*this); return retval; }
            RowElementIterator& operator--() { p.x--; return *this; }
            RowElementIterator operator--(int) {auto retval = *this; --(*this); return retval; }
            bool operator==(const RowElementIterator& other) const { return p == other.p; }
            bool operator!=(const RowElementIterator& other) const { return !(*this == other); }
            auto& operator*() const { return grid->at(p); }
            auto& operator*() { return grid->at(p); }

            difference_type operator-(const RowElementIterator& other) const {
                assertEquals(grid, other.grid);
                assertEquals(p.y, other.p.y);
                return static_cast<difference_type>(p.x) - static_cast<difference_type>(p.x);
            }
            RowElementIterator& operator+=(const difference_type diff) {
                p.x += diff;
                return *this;
            }
            RowElementIterator& operator-=(const difference_type diff) {
                p.x -= diff;
                return *this;
            }
        };

        template <typename GridT>
        struct Row {
            GridT * const grid;
            const size_t row;
            Row(GridT * const grid, const size_t row): grid{grid}, row{row} {};

            auto begin() const {
                return RowElementIterator{grid, 0, row};
            }

            auto end() const {
                return RowElementIterator{grid, grid->data.size(), row};
            };


            auto rbegin() const {
                return std::reverse_iterator{end()};
            }

            auto rend() const {
                return reverse_iterator{begin()};
            }
        };

        template <typename GridT>
        struct RowIterator {
            using iterator_category = std::random_access_iterator_tag;
            using value_type = Row<GridT>;
            using difference_type = std::ptrdiff_t;
            using pointer = value_type*;
            using reference = value_type&;

            GridT * grid;
            size_t row;
            explicit constexpr RowIterator(): RowIterator{nullptr, 0} {}
            explicit constexpr RowIterator(GridT * const grid, const size_t row): grid{grid}, row{row} {}
            constexpr RowIterator(const RowIterator&) = default;
            constexpr RowIterator& operator=(const RowIterator& other) {
                grid = other.grid;
                row = other.row;
                return *this;
            }
            constexpr RowIterator& operator=(RowIterator&& other) {
                grid = other.grid;
                row = other.row;
                return *this;
            }
            RowIterator& operator++() { row++; return *this; }
            RowIterator operator++(int) {auto retval = *this; ++(*this); return retval; }
            RowIterator& operator--() { row--; return *this; }
            RowIterator operator--(int) {auto retval = *this; --(*this); return retval; }
            bool operator==(const RowIterator& other) const { return row == other.row; }
            bool operator!=(const RowIterator& other) const { return !(*this == other); }
            auto operator*() const { return Row{grid, row}; }
            auto operator*() { return Row{grid, row}; }

            difference_type operator-(const RowIterator& other) const {
                return static_cast<difference_type>(row) - static_cast<difference_type>(other.row);
            }

            RowIterator operator+(const difference_type diff) const {
                auto retval = *this;
                retval.row += diff;
                return retval;
            }
            RowIterator operator-(const difference_type diff) const {
                auto retval = *this;
                retval.row -= diff;
                return retval;
            }
            RowIterator& operator+=(const difference_type diff) {
                row += diff;
                return *this;
            }
            RowIterator& operator-=(const difference_type diff) {
                row -= diff;
                return *this;
            }
        };

        auto beginRows() {
            return RowIterator{this, 0ul};
        }
        
        auto beginRows() const {
            return RowIterator{this, 0ul};
        }

        auto endRows() {
            return RowIterator{this, this->data.front().size()};
        }

        auto endRows() const {
            return RowIterator{this, this->data.front().size()};
        }

        /** Row element access pattern */
        template <typename GridT>
        struct ColumnElementIterator {
            using iterator_category = std::random_access_iterator_tag;
            using value_type = GridT::value_type;
            using difference_type = std::ptrdiff_t;
            using pointer = value_type*;
            using reference = value_type&;

            GridT * grid;
            GenericPoint<size_t> p;
            explicit constexpr ColumnElementIterator() : ColumnElementIterator{nullptr, 0} {}
            explicit constexpr ColumnElementIterator(GridT * const grid, const size_t column, const size_t row)
                : grid(grid)
                , p(column, row)
            {
            }
            constexpr ColumnElementIterator(const ColumnElementIterator&) = default;
            constexpr ColumnElementIterator& operator=(const ColumnElementIterator& other) {
                grid = other.grid;
                p = other.p;
                return *this;
            }
            constexpr ColumnElementIterator& operator=(ColumnElementIterator&& other) {
                grid = other.grid;
                p = other.p;
                return *this;
            }
            ColumnElementIterator& operator++() { p.y++; return *this; }
            ColumnElementIterator operator++(int) {auto retval = *this; ++(*this); return retval; }
            ColumnElementIterator& operator--() { p.y--; return *this; }
            ColumnElementIterator operator--(int) {auto retval = *this; --(*this); return retval; }
            bool operator==(const ColumnElementIterator& other) const { return p == other.p; }
            bool operator!=(const ColumnElementIterator& other) const { return !(*this == other); }
            auto& operator*() const { return grid->at(p); }
            auto& operator*() { return grid->at(p); }

            difference_type operator-(const ColumnElementIterator& other) const {
                assertEquals(grid, other.grid);
                assertEquals(p.x, other.p.x);
                return static_cast<difference_type>(p.y) - static_cast<difference_type>(other.p.y);
            }

            ColumnElementIterator operator+(const difference_type diff) const {
                auto retval = *this;
                retval.p.y += diff;
                return retval;
            }
            ColumnElementIterator operator-(const difference_type diff) const {
                auto retval = *this;
                retval.p.y -= diff;
                return retval;
            }
            ColumnElementIterator& operator+=(const difference_type diff) {
                p.y += diff;
                return *this;
            }
            ColumnElementIterator& operator-=(const difference_type diff) {
                p.y -= diff;
                return *this;
            }
        };

        template <typename GridT>
        struct Column {
            GridT * const grid;
            const size_t column;
            Column(GridT * const grid, const size_t column): grid{grid}, column{column} {};

            auto begin() const {
                return ColumnElementIterator{grid, column, 0};
            }

            auto end() const {
                return ColumnElementIterator{grid, column, grid->data.size()};
            }

            auto rbegin() const {
                return std::reverse_iterator{end()};
            }

            auto rend() const {
                return std::reverse_iterator{begin()};
            }
        };

        template <typename GridT>
        struct ColumnIterator {
            using iterator_category = std::random_access_iterator_tag;
            using value_type = Column<GridT>;
            using difference_type = std::ptrdiff_t;
            using pointer = value_type*;
            using reference = value_type&;

            GridT * grid;
            size_t column;
            explicit constexpr ColumnIterator() : ColumnIterator{nullptr, 0} {}
            explicit constexpr ColumnIterator(GridT * const grid, const size_t column): grid{grid}, column{column} {}
            constexpr ColumnIterator(const ColumnIterator&) = default;
            constexpr ColumnIterator& operator=(const ColumnIterator& other) {
                grid = other.grid;
                column = other.column;
                return *this;
            }
            ColumnIterator& operator=(ColumnIterator&& other) {
                grid = other.grid;
                column = other.column;
                return *this;
            }
            ColumnIterator& operator++() { column++; return *this; }
            ColumnIterator operator++(int) {auto retval = *this; ++(*this); return retval; }
            ColumnIterator& operator--() { column--; return *this; }
            ColumnIterator operator--(int) {auto retval = *this; --(*this); return retval; }
            bool operator==(const ColumnIterator other) const { return column == other.column; }
            bool operator!=(const ColumnIterator other) const { return !(*this == other); }
            auto operator*() const { return Column<GridT>{grid, column}; }
            auto operator*() { return Column<GridT>{grid, column}; }
            // auto operatpr->() const { ??? }

            difference_type operator-(const ColumnIterator& other) const {
                return static_cast<difference_type>(column) - static_cast<difference_type>(other.column);
            }

            ColumnIterator operator+(const difference_type diff) const {
                auto retval = *this;
                retval.column += diff;
                return retval;
            }
            ColumnIterator operator-(const difference_type diff) const {
                auto retval = *this;
                retval.column += diff;
                return retval;
            }
            ColumnIterator& operator+=(const difference_type diff) {
                column += diff;
                return *this;
            }
            ColumnIterator& operator-=(const difference_type diff) {
                column -= diff;
                return *this;
            }
        };

        auto beginColumns() {
            return ColumnIterator{this, 0};
        }

        auto endColumns() {
            return ColumnIterator{this, this->data.front().size()};
        }

        auto beginColumns() const {
            return ColumnIterator{this, 0};
        }

        auto endColumns() const {
            return ColumnIterator{this, this->data.front().size()};
        }

        auto rows() {
            return std::ranges::subrange/*<
                Grid<T>::RowIterator<const Grid<T>>,
                Grid<T>::RowIterator<const Grid<T>>,
                std::ranges::subrange_kind::sized>*/
                {beginRows(), endRows()};
        }
        auto rows() const { return std::ranges::subrange {beginRows(), endRows()}; }
        auto columns() { return std::ranges::subrange {beginColumns(), endColumns()}; }
        auto columns() const { return std::ranges::subrange {beginColumns(), endColumns()}; }

        // row-wise check
        bool operator==(const Grid<T>& other) const {
            const auto [ours, others] = mismatch(
                beginRows(), endRows(),
                other.beginRows(), other.endRows(),
                [] (const Row<const Grid<T>>& ourRow, const Row<const Grid<T>>& otherRow) -> bool {
                    const auto [ours, others] = mismatch(
                        ourRow.begin(), ourRow.end(),
                        otherRow.begin(), otherRow.end());
                    return ours == ourRow.end() && others == otherRow.end();
                });
            return ours == endRows() && others == other.endRows();
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

        bool hit(const Point& p) const{
            return topLeft.x <= p.x &&
                topLeft.y <= p.y &&
                p.x <= bottomRight.x &&
                p.y <= bottomRight.y;
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

    template <typename T>
    T parseNumber(const string_view input, const int base = 10) {
        T value;
        const auto [ptr, ec] = from_chars(input.begin(), input.end(), value, base);
        if(ec == errc{}){
            return value;
        }

        switch(ec) {
            case errc::invalid_argument:
                throw invalid_argument("Failed to parse a number from ["s + string(input) + "]");

            case errc::result_out_of_range:
                throw range_error(
                    "Failed to parse a number from ["s
                    + string(input)
                    + "] at position "
                    + to_string(distance(input.begin(), ptr)));

            default:
                // undefined error
                throw runtime_error("Failed to parse a number from ["s + string(input) + "]: " + to_string(static_cast<long>(ec)));
        }
    }
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const utils::Grid<T>& grid);

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
    os << "Grid(" << grid.data.size() << "x" << grid.data.front().size() << ")\n";
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
