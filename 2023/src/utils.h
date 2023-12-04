#ifndef AOC_UTILS_h
#define AOC_UTILS_h

#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <utility> // pair

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
}

void assertEquals(const auto& expected, const auto& actual) {
    if(expected != actual) {
        std::cerr << "Expected: " << expected << "\n" << "Actual: " << actual << std::endl;
        throw std::runtime_error("Expected value does not equal to the actual one");
    }
}

template <typename T> 
std::ostream& operator<<(std::ostream& os, std::vector<T> input){
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


#endif
