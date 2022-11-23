#pragma once

#include <string>
#include "object.h"
#include "parser.h"

class Interpreter {
public:
    Interpreter() {
        current_scope = heap.Clone(&global);
    }

    std::string Run(const std::string&);

private:
    std::string Serialize(Object* cur_node);
    std::string SerializeList(Object* cur_node);
};
