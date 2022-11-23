#pragma once

#include <string>
#include "object.h"
#include "parser.h"

class Interpreter {
public:
    Interpreter() {
        current_scope = std::make_shared<Scope>(global);
    }

    std::string Run(const std::string&);

private:
    std::string Serialize(std::shared_ptr<Object>);
    std::string SerializeList(std::shared_ptr<Object> cur_node);
};
