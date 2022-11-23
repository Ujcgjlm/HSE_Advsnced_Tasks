#include <alloca.h>
#include <parser.h>
#include <memory>
#include "error.h"
#include "object.h"
#include "tokenizer.h"

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    if (tokenizer->GetToken() == Token(DotToken())) {
        tokenizer->Next();
        auto last = Read(tokenizer);

        if (tokenizer->IsEnd() || tokenizer->GetToken() != Token{BracketToken::CLOSE}) {
            throw SyntaxError("");
        }
        tokenizer->Next();

        return last;
    }
    if (tokenizer->GetToken() == Token{BracketToken::CLOSE}) {
        tokenizer->Next();
        return nullptr;
    } else {
        auto left = Read(tokenizer);
        auto right = ReadList(tokenizer);
        return std::make_shared<Cell>(left, right);
    }
}

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    if (!tokenizer->IsEnd()) {
        auto cur_token = tokenizer->GetToken();
        tokenizer->Next();

        if (cur_token == Token(QuoteToken())) {
            throw SyntaxError("");
        }

        if (cur_token == Token{BracketToken::OPEN}) {
            return ReadList(tokenizer);
        }

        if (std::get_if<SymbolToken>(&cur_token)) {
            return std::make_shared<Symbol>(std::get<SymbolToken>(cur_token).name);
        }
        if (std::get_if<ConstantToken>(&cur_token)) {
            return std::make_shared<Number>(std::get<ConstantToken>(cur_token).value);
        }
    }
    throw SyntaxError("");
}
