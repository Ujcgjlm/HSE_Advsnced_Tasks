#include <tokenizer.h>
#include <iostream>

#include "error.h"

bool SymbolToken::operator==(const SymbolToken& other) const {
    return name == other.name;
}

bool QuoteToken::operator==(const QuoteToken&) const {
    return true;
}

bool DotToken::operator==(const DotToken&) const {
    return true;
}

bool ConstantToken::operator==(const ConstantToken& other) const {
    return value == other.value;
}

bool BoolToken::operator==(const BoolToken& other) const {
    return value == other.value;
}

Tokenizer::Tokenizer(std::istream* in) : in_stream_(in) {
    Next();
}

bool Tokenizer::IsEnd() {
    return is_end_;
}

void Tokenizer::Next() {
    while (std::isspace(in_stream_->peek())) {
        in_stream_->get();
    }

    if (in_stream_->peek() == EOF) {
        if (is_end_) {
            throw SyntaxError("");
        }
        is_end_ = true;
        return;
    }

    char c = in_stream_->get();
    switch (c) {
        case '.':
            current_ = Token(DotToken());
            return;
        case '\'':
            current_ = Token(QuoteToken());
            return;
        case '(':
            current_ = Token(BracketToken::OPEN);
            return;
        case ')':
            current_ = Token(BracketToken::CLOSE);
            return;
        case '+':
            if (!std::isdigit(in_stream_->peek())) {
                current_ = Token(SymbolToken{"+"});
                return;
            }
            break;
        case '-':
            if (!std::isdigit(in_stream_->peek())) {
                current_ = Token(SymbolToken{"-"});
                return;
            }
            break;
        default:
            break;
    }

    if (std::isdigit(c) || c == '-' || c == '+') {
        int64_t sign = (c == '-') ? -1 : 1;
        if (c == '-' || c == '+') {
            c = in_stream_->get();
        }
        current_ = Token(ConstantToken{c - '0'});
        auto& value = std::get<ConstantToken>(current_).value;

        while (std::isdigit(in_stream_->peek()) && in_stream_->peek() != EOF) {
            value = value * 10 + in_stream_->get() - '0';
        }

        value *= sign;

        return;
    }

    current_ = Token(SymbolToken{std::string(1, c)});
    auto& name = std::get<SymbolToken>(current_).name;

    while (!std::isspace(in_stream_->peek()) && in_stream_->peek() != ')' &&
           in_stream_->peek() != EOF) {
        name.push_back(in_stream_->get());
    }

    if (!std::regex_match(name, symbols_token_regex_)) {
        throw SyntaxError("");
    }

    if (name == "#f") {
        current_ = Token(BoolToken{false});
    } else {
        if (name == "#t") {
            current_ = Token(BoolToken{true});
        }
    }
}

Token Tokenizer::GetToken() {
    return current_;
}
