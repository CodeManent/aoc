#ifndef ParsingError_h_
#define ParsingError_h_

#include <stdexcept>
#include <string>

class ParsingError : public std::runtime_error {
public:
    ParsingError(const std::string& reason)
        :runtime_error(reason)
    {
    }
};

#endif // !ParsingError_h_