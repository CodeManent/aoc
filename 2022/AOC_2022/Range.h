#ifndef Range_h_
#define Range_h_

class Range {
public:
    int start;
    int end;

    Range(int start, int end);

    bool contains(const int point) const;
    bool contains(const Range& other) const;
    bool overlaps(const Range& other) const;
};

#endif

