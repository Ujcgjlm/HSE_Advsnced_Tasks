#pragma once

#include <memory>
#include <variant>
#include <optional>
#include <istream>
#include <regex>

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken& other) const;
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const;
};

struct DotToken {
    bool operator==(const DotToken&) const;
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int64_t value;

    bool operator==(const ConstantToken& other) const;
};

struct BoolToken {
    bool value;

    bool operator==(const BoolToken& other) const;
};

using Token =
    std::variant<BoolToken, ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken>;

class Tokenizer {
public:
    Tokenizer(std::istream* in);

    bool IsEnd();

    void Next();

    Token GetToken();

private:
    std::istream* in_stream_;
    Token current_;
    bool is_end_ = false;
    std::regex symbols_token_regex_{"^[a-zA-Z<=>*/#]+[a-zA-Z<=>*/#0-9?!-]*"};
};
