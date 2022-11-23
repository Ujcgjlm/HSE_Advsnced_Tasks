#pragma once

#include <cstddef>
#include <memory>
#include <unordered_map>
#include "error.h"
#include "tokenizer.h"

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number(int64_t value) : value_(value) {
    }

    int64_t GetValue() const {
        return value_;
    }

private:
    int64_t value_;
};

class Bool : public Object {
public:
    Bool(int64_t value) : value_(value) {
    }

    int GetValue() const {
        return value_;
    }

private:
    bool value_;
};

class Symbol : public Object {
public:
    Symbol(std::string name) : name_(std::move(name)) {
    }

    const std::string& GetName() const {
        return name_;
    }

private:
    std::string name_;
};

class Cell : public Object {
public:
    Cell(std::shared_ptr<Object> left, std::shared_ptr<Object> right) : left_(left), right_(right) {
    }

    std::shared_ptr<Object> GetFirst() const {
        return left_;
    }

    std::shared_ptr<Object> GetSecond() const {
        return right_;
    }

private:
    std::shared_ptr<Object> left_;
    std::shared_ptr<Object> right_;
};

static std::vector<std::shared_ptr<Object>> CreateVectorFromList(std::shared_ptr<Object> cur_node);
static std::shared_ptr<Object> Eval(std::shared_ptr<Object> cur_node);
static std::vector<std::shared_ptr<Object>> EvalList(std::shared_ptr<Object> cur_node);

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and convertion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj) != nullptr;
}

// Function

class Function : public Object {
public:
    virtual ~Function() = default;
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) = 0;
};

// Quote

class Quote : public Function {
public:
    virtual ~Quote() = default;
    virtual std::shared_ptr<Object> operator()(const std::vector<std::shared_ptr<Object>>& params) {
        return params[0];
    }
};

class QuoteFunction : public Quote {
public:
    virtual ~QuoteFunction() = default;
    virtual std::shared_ptr<Object> operator()(const std::vector<std::shared_ptr<Object>>& params) {
        if (params.size() != 1) {
            throw RuntimeError("");
        }
        return As<Cell>(params[0])->GetFirst();
    }
};

// IntegerFunctions

class IntegerFunction : public Function {
public:
    void Check(const std::vector<std::shared_ptr<Object>>& params) {
        for (auto param : params) {
            if (!param || !Is<Number>(param)) {
                throw RuntimeError("");
            }
        }
    }
};

struct IsNumber : public IntegerFunction {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {

        for (auto param : params) {
            if (!param || !Is<Number>(param)) {
                return std::make_shared<Bool>(false);
            }
        }

        return std::make_shared<Bool>(true);
    }
};

struct Sum : public IntegerFunction {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        Check(params);

        int64_t ans = 0;
        for (auto param : params) {
            ans += As<Number>(param)->GetValue();
        }
        return std::make_shared<Number>(ans);
    }
};

struct Difference : public IntegerFunction {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        Check(params);

        if (params.empty()) {
            throw RuntimeError("");
        }

        int64_t ans = As<Number>(params[0])->GetValue();
        for (size_t i = 1; i < params.size(); ++i) {
            ans -= As<Number>(params[i])->GetValue();
        }
        return std::make_shared<Number>(ans);
    }
};

struct Multiplication : public IntegerFunction {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        Check(params);

        int64_t ans = 1;
        for (auto param : params) {
            ans *= As<Number>(param)->GetValue();
        }
        return std::make_shared<Number>(ans);
    }
};

struct Division : public IntegerFunction {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        Check(params);

        if (params.empty()) {
            throw RuntimeError("");
        }

        int64_t ans = As<Number>(params[0])->GetValue();
        for (size_t i = 1; i < params.size(); ++i) {
            ans /= As<Number>(params[i])->GetValue();
        }
        return std::make_shared<Number>(ans);
    }
};

struct Equality : public IntegerFunction {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        Check(params);

        for (size_t i = 1; i < params.size(); ++i) {
            if (As<Number>(params[i])->GetValue() != As<Number>(params[0])->GetValue()) {
                return std::make_shared<Bool>(false);
            }
        }
        return std::make_shared<Bool>(true);
    }
};

struct StrictlyDescending : public IntegerFunction {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        Check(params);

        for (size_t i = 1; i < params.size(); ++i) {
            if (!(As<Number>(params[i - 1])->GetValue() > As<Number>(params[i])->GetValue())) {
                return std::make_shared<Bool>(false);
            }
        }
        return std::make_shared<Bool>(true);
    }
};

struct StrictlyAscending : public IntegerFunction {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        Check(params);

        for (size_t i = 1; i < params.size(); ++i) {
            if (!(As<Number>(params[i - 1])->GetValue() < As<Number>(params[i])->GetValue())) {
                return std::make_shared<Bool>(false);
            }
        }
        return std::make_shared<Bool>(true);
    }
};

struct Descending : public IntegerFunction {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        Check(params);

        for (size_t i = 1; i < params.size(); ++i) {
            if (!(As<Number>(params[i - 1])->GetValue() >= As<Number>(params[i])->GetValue())) {
                return std::make_shared<Bool>(false);
            }
        }
        return std::make_shared<Bool>(true);
    }
};

struct Ascending : public IntegerFunction {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        Check(params);

        for (size_t i = 1; i < params.size(); ++i) {
            if (!(As<Number>(params[i - 1])->GetValue() <= As<Number>(params[i])->GetValue())) {
                return std::make_shared<Bool>(false);
            }
        }
        return std::make_shared<Bool>(true);
    }
};

struct Abs : public IntegerFunction {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        if (params.size() != 1) {
            throw RuntimeError("");
        }

        Check(params);

        return std::make_shared<Number>(std::abs(As<Number>(params.front())->GetValue()));
    }
};

struct Max : public IntegerFunction {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        Check(params);

        if (params.empty()) {
            throw RuntimeError("");
        }

        int64_t ans = As<Number>(params[0])->GetValue();

        for (auto param : params) {
            ans = std::max(ans, As<Number>(param)->GetValue());
        }

        return std::make_shared<Number>(ans);
    }
};

struct Min : public IntegerFunction {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        Check(params);

        if (params.empty()) {
            throw RuntimeError("");
        }

        int64_t ans = As<Number>(params[0])->GetValue();

        for (auto param : params) {
            ans = std::min(ans, As<Number>(param)->GetValue());
        }

        return std::make_shared<Number>(ans);
    }
};

// BoolFunctions

class BoolFunction : public Function {
public:
    std::shared_ptr<Object> ParamToBool(std::shared_ptr<Object> param) {
        param = Eval(param);
        return Is<Bool>(param) ? param : std::make_shared<Bool>(true);
    }
};

class IsBool : public BoolFunction {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        for (auto param : params) {
            if (!param || !Is<Bool>(param)) {
                return std::make_shared<Bool>(false);
            }
        }
        return std::make_shared<Bool>(true);
    }
};

class Or : public BoolFunction {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        for (auto param : params) {
            if (As<Bool>(ParamToBool(param))->GetValue()) {
                return Eval(param);
            }
        }
        return std::make_shared<Bool>(false);
    }
};

class And : public BoolFunction {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        for (auto param : params) {
            if (!As<Bool>(ParamToBool(param))->GetValue()) {
                return std::make_shared<Bool>(false);
            }
        }
        if (!params.empty()) {
            return Eval(params.back());
        }
        return std::make_shared<Bool>(true);
    }
};

class Not : public BoolFunction {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        if (params.size() != 1) {
            throw RuntimeError("");
        }
        return std::make_shared<Bool>(!As<Bool>(ParamToBool(params[0]))->GetValue());
    }
};

// List Functions

class Car : public Function {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        if (params.empty() || !Is<Cell>(params[0]) || params.size() != 1) {
            throw RuntimeError("");
        }
        return As<Cell>(params[0])->GetFirst();
    }
};

class Cdr : public Function {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        if (params.empty() || !Is<Cell>(params[0]) || params.size() != 1) {
            throw RuntimeError("");
        }
        return As<Cell>(params[0])->GetSecond();
    }
};

class Cons : public Function {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        if (params.size() != 2) {
            throw RuntimeError("");
        }
        return std::make_shared<Cell>(params[0], params[1]);
    }
};

class MakeList : public Function {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        if (params.empty()) {
            return nullptr;
        }
        return std::make_shared<Cell>(params[0], MakeObjectFromList(params));
    }

private:
    std::shared_ptr<Object> MakeObjectFromList(const std::vector<std::shared_ptr<Object>>& params,
                                               size_t pos = 1) {
        if (pos == params.size()) {
            return nullptr;
        }
        return std::make_shared<Cell>(params[pos], MakeObjectFromList(params, pos + 1));
    }
};

class IsList : public Function {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        if (params.size() != 1) {
            throw RuntimeError("");
        }
        if (!Is<Cell>(params[0]) && params[0]) {
            return std::make_shared<Bool>(false);
        }
        return std::make_shared<Bool>(Checker(params[0]));
    }

private:
    bool Checker(const std::shared_ptr<Object>& param) {
        if (!param) {
            return true;
        }
        if (!Is<Cell>(param)) {
            return false;
        }
        return Checker(As<Cell>(param)->GetSecond());
    }
};

class IsPair : public Function {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        if (params.size() != 1) {
            throw RuntimeError("");
        }
        return std::make_shared<Bool>(CreateVectorFromList(params[0]).size() == 2);
    }
};

class IsNull : public Function {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        if (params.size() != 1) {
            throw RuntimeError("");
        }
        return std::make_shared<Bool>(!params[0]);
    }
};

class GetListRef : public Function {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        if (params.size() != 2 || !Is<Cell>(params[0]) || !Is<Number>(params[1])) {
            throw RuntimeError("");
        }
        auto n = As<Number>(params[1])->GetValue();
        auto cur_node = params[0];
        for (int64_t i = 0; i < n; ++i) {
            if (!Is<Cell>(cur_node) || !cur_node) {
                throw RuntimeError("");
            }
            cur_node = As<Cell>(cur_node)->GetSecond();
        }
        if (!Is<Cell>(cur_node) || !cur_node) {
            throw RuntimeError("");
        }
        return As<Cell>(cur_node)->GetFirst();
    }
};

class GetListTail : public Function {
public:
    virtual std::shared_ptr<Object> operator()(
        const std::vector<std::shared_ptr<Object>>& params) override {
        if (params.size() != 2 || !Is<Cell>(params[0]) || !Is<Number>(params[1])) {
            throw RuntimeError("");
        }
        auto n = As<Number>(params[1])->GetValue();
        auto cur_node = params[0];
        for (int64_t i = 0; i < n; ++i) {
            if (!Is<Cell>(cur_node) || !cur_node) {
                throw RuntimeError("");
            }
            cur_node = As<Cell>(cur_node)->GetSecond();
        }
        return cur_node;
    }
};

static const std::unordered_map<std::string, std::shared_ptr<Object>> kObjectFromSymbol = {
    // Quote
    {"'", std::make_shared<Quote>()},
    {"quote", std::make_shared<QuoteFunction>()},

    // IntegerFunctions
    {"+", std::make_shared<Sum>()},
    {"-", std::make_shared<Difference>()},
    {"*", std::make_shared<Multiplication>()},
    {"/", std::make_shared<Division>()},
    {"=", std::make_shared<Equality>()},
    {"<", std::make_shared<StrictlyAscending>()},
    {">", std::make_shared<StrictlyDescending>()},
    {"<=", std::make_shared<Ascending>()},
    {">=", std::make_shared<Descending>()},
    {"abs", std::make_shared<Abs>()},
    {"min", std::make_shared<Min>()},
    {"max", std::make_shared<Max>()},
    {"number?", std::make_shared<IsNumber>()},

    // BooleFunctions
    {"or", std::make_shared<Or>()},
    {"and", std::make_shared<And>()},
    {"not", std::make_shared<Not>()},
    {"boolean?", std::make_shared<IsBool>()},

    // ListFunctions
    {"car", std::make_shared<Car>()},
    {"cdr", std::make_shared<Cdr>()},
    {"cons", std::make_shared<Cons>()},
    {"list", std::make_shared<MakeList>()},
    {"list?", std::make_shared<IsList>()},
    {"pair?", std::make_shared<IsPair>()},
    {"null?", std::make_shared<IsNull>()},
    {"list-ref", std::make_shared<GetListRef>()},
    {"list-tail", std::make_shared<GetListTail>()}};

static std::vector<std::shared_ptr<Object>> CreateVectorFromList(std::shared_ptr<Object> cur_node) {
    if (!cur_node) {
        return {};
    }
    if (!Is<Cell>(cur_node)) {
        return {cur_node};
    }
    std::vector<std::shared_ptr<Object>> vec1 = {As<Cell>(cur_node)->GetFirst()};
    std::vector<std::shared_ptr<Object>> vec2 =
        CreateVectorFromList(As<Cell>(cur_node)->GetSecond());
    for (auto obj : vec2) {
        vec1.push_back(obj);
    }
    return vec1;
}

static std::vector<std::shared_ptr<Object>> EvalList(std::shared_ptr<Object> cur_node) {
    if (!cur_node) {
        return {};
    }
    if (!Is<Cell>(cur_node)) {
        return {Eval(cur_node)};
    }
    std::vector<std::shared_ptr<Object>> vec1 = {Eval(As<Cell>(cur_node)->GetFirst())};
    std::vector<std::shared_ptr<Object>> vec2 = EvalList(As<Cell>(cur_node)->GetSecond());
    for (auto obj : vec2) {
        vec1.push_back(obj);
    }
    return vec1;
}

static std::shared_ptr<Object> Eval(std::shared_ptr<Object> cur_node) {
    if (!cur_node) {
        throw RuntimeError("");
    }

    if (Is<Number>(cur_node) || Is<Bool>(cur_node)) {
        return cur_node;
    }

    if (Is<Symbol>(cur_node)) {
        if (!kObjectFromSymbol.contains(As<Symbol>(cur_node)->GetName())) {
            throw RuntimeError("");
        }
        return kObjectFromSymbol.at(As<Symbol>(cur_node)->GetName());
    }

    // Cell
    auto cell = As<Cell>(cur_node);
    auto calculated_function = Eval(cell->GetFirst());

    if (!Is<Function>(calculated_function)) {
        throw RuntimeError("");
    }

    if (Is<Quote>(calculated_function)) {
        return (*As<Quote>(calculated_function))({cell->GetSecond()});
    }

    if (Is<BoolFunction>(calculated_function)) {
        return (*As<BoolFunction>(calculated_function))(CreateVectorFromList(cell->GetSecond()));
    }

    return (*As<Function>(calculated_function))(EvalList(cell->GetSecond()));
}
