#include "day.h"
#include "utils.h"

#include "ParsingError.h"
#include "Filesystem.h"
#include "FilesystemPrinter.h"

#include "DirectorySizeComputer.h"
#include "FilteredDirSizeReporter.h"
#include "DeleteDirectorySelector.h"

#include <sstream>
#include <vector>
#include <string>

using namespace std;

Filesystem parseConsoleInput(const vector<string>& input) {
    Filesystem fs;

    for (auto line = input.begin(); line != input.end(); ++line) {
        istringstream iss(*line);
        string token;
        iss >> token;
        if (token != "$") {
            throw ParsingError("Not a command");
        }

        iss >> token;
        if (token == "cd") {
            string dirToken;
            iss >> dirToken;
            fs.cd(dirToken);
        }
        if (token == "ls") {
            // parse response lines until a command lins is found
            while ((line + 1) != input.end() && (line + 1)->at(0) != '$') {
                ++line;

                // parse ls response line
                string lsToken1, lsToken2;
                istringstream iss2(*line);
                iss2 >> lsToken1 >> lsToken2;
                if (lsToken1 == "dir") {
                    fs.addDir(lsToken2);
                }
                else {
                    const size_t size = atoi(lsToken1.data());
                    fs.addFile(lsToken2, size);
                }
            }
        }


    }


    return fs;
}


DayResult day7() {
    const auto consoleLog = readFile("day_7_input.txt");
    auto fs = parseConsoleInput(consoleLog);

    fs.cd("/");

    //FilesystemPrinter printer;
    //fs.accept(printer);

    FilteredDirSizeReporter reporter;
    DirectorySizeComputer visitor(reporter);

    fs.accept(visitor);

    DeleteDirectorySelector selector(visitor.dirSize);
    DirectorySizeComputer visitor2(selector);

    fs.accept(visitor2);

    if (!selector.selected.has_value()) {
        throw runtime_error("No dir was selected for deletion");
    }

    return {
        make_optional<PartialDayResult>({"Reported (filtered) accumulated size", to_string(reporter.accumulatedSize)}),
        make_optional<PartialDayResult>({"Size of dir (" + selector.selected->name + ") selected for deletion", to_string(selector.selected->size)})
    };
}
