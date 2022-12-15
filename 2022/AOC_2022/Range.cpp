#include "Range.h"

Range::Range(int start, int end)
    :start(start)
    , end(end)
{
}

bool Range::contains(const int point) const {
    return start <= point && point <= end;
}

bool Range::contains(const Range& other) const {
    return start <= other.start && other.end <= end;
}

bool Range::overlaps(const Range& other) const {
    return contains(other) || other.contains(*this) || contains(other.start) || contains(other.end);
}
