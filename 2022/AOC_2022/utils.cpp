#include "utils.h"

#include <fstream>
#include <string>

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