#include "scheme.h"
#include <string>

#include "error.h"
#include "object.h"
#include "tokenizer.h"

std::string Interpreter::SerializeList(Object* cur_node) {
    if (!cur_node) {
        return "";
    }
    if (!Is<Cell>(cur_node)) {
        return ". " + Serialize(cur_node);
    }
    auto second_part = SerializeList(As<Cell>(cur_node)->GetSecond());
    return Serialize(As<Cell>(cur_node)->GetFirst()) +
           ((!second_part.empty()) ? (" " + second_part) : "");
}

std::string Interpreter::Serialize(Object* cur_node) {
    if (Is<Cell>(cur_node)) {
        return "(" + SerializeList(cur_node) + ")";
    }

    if (Is<Symbol>(cur_node)) {
        return As<Symbol>(cur_node)->GetName();
    }
    if (Is<Number>(cur_node)) {
        return std::to_string(As<Number>(cur_node)->GetValue());
    }
    if (Is<Bool>(cur_node)) {
        return As<Bool>(cur_node)->GetValue() ? "#t" : "#f";
    }
    return "()";
}

std::string Interpreter::Run(const std::string& str) {
    std::stringstream ss{str};
    Tokenizer tokenizer{&ss};
    auto ast = Read(&tokenizer);
    if (!tokenizer.IsEnd()) {
        throw SyntaxError("");
    }
    auto calculated_ast = Eval(ast);
    auto ans = Serialize(calculated_ast);
    heap.MarkAndSweep();
    return ans;
}
