#include "day.h"
#include "utils.h"

#include "Grid.h"

#include "printers.h"

using namespace std;

DayResult day8() {
    const auto input = readFile("day_8_input.txt");

    const size_t n = std::max(input[0].length(), input.size());
    const auto inputLocationMapper = [&input](const SizePoint& p) -> Grid::Location {
        return input[p.y][p.x] - '0';
    };

    Grid grid(n, inputLocationMapper);
    grid.computeVisibility();
    // cout << "Grid:\n" << grid << endl;

    size_t visibleCount = 0;
    long maxScenicScore = 0;

    grid.visitTopDownLeftRight([&visibleCount, &maxScenicScore](auto, const Grid::Location& current) {
        if (current.isVisible) {
            ++visibleCount;
        }
        maxScenicScore = std::max(maxScenicScore, current.scenicScore);
    });

    return {
        make_optional<PartialDayResult>({"Visible trees", to_string(visibleCount)}),
        make_optional<PartialDayResult>({"Max scenic score", to_string(maxScenicScore)})
    };
}
