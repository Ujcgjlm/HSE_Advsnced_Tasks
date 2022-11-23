#pragma once

#include <string>
#include "parser.h"

class Interpreter {
public:
    std::string Run(const std::string&);

private:
    std::string Serialize(std::shared_ptr<Object>);
    std::string SerializeList(std::shared_ptr<Object> cur_node);
};
