#ifndef StackCommand_h_
#define StackCommand_h_

#include <string>

class StackCommand {
public:
    std::string source;
    std::string target;
    int amount;

    StackCommand(std::string source, std::string target, int amount)
        :source(source)
        , target(target)
        , amount(amount)
    {
    }
};

#endif // StackCommand_h_
