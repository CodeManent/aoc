#ifndef TaggedStack_h_
#define TaggedStack_h_

#include <string>
#include <stack>
#include <ostream>

class TaggedStack {
public:
    const std::string tag;
    std::stack<char> stack;

    TaggedStack(const std::string& tag)
        :tag(tag)
    {
    }

    friend std::ostream& operator<< (std::ostream&, const TaggedStack&);
};

#endif
