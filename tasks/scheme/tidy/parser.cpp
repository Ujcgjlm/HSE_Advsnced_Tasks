#include <alloca.h>
#include <parser.h>
#include <memory>
#include "error.h"
#include "object.h"
#include "tokenizer.h"

Object* ReadList(Tokenizer* tokenizer) {
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
        return heap.Make<Cell>(left, right);
    }
}

Object* Read(Tokenizer* tokenizer) {
    if (!tokenizer->IsEnd()) {
        auto cur_token = tokenizer->GetToken();
        tokenizer->Next();

        if (cur_token == Token(QuoteToken())) {
            return heap.Make<Cell>(heap.Make<Symbol>("'"), Read(tokenizer));
        }

        if (cur_token == Token{BracketToken::OPEN}) {
            if (tokenizer->IsEnd() || tokenizer->GetToken() == Token(DotToken{})) {
                throw SyntaxError("");
            }
            return ReadList(tokenizer);
        }

        if (std::get_if<SymbolToken>(&cur_token)) {
            return heap.Make<Symbol>(std::get<SymbolToken>(cur_token).name);
        }
        if (std::get_if<ConstantToken>(&cur_token)) {
            return heap.Make<Number>(std::get<ConstantToken>(cur_token).value);
        }
        if (std::get_if<BoolToken>(&cur_token)) {
            return heap.Make<Bool>(std::get<BoolToken>(cur_token).value);
        }
    }
    throw SyntaxError("");
}
