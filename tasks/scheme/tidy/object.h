#pragma once

#include <cstddef>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <unordered_map>
#include "error.h"
#include "tokenizer.h"

class Object {
public:
    virtual ~Object() = default;
    virtual void Dfs() {
        mark_ = true;
    }

    bool mark_ = false;
};

class Scope : public Object {
public:
    Scope() = delete;

    Scope(Scope* parent_scope, const std::unordered_map<std::string, Object*>& scope = {})
        : parent_scope_(parent_scope), scope_(scope) {
    }

    Scope(const Scope& scope) : parent_scope_(scope.parent_scope_), scope_(scope.scope_) {
    }

    Object*& GetVariableInScopes(const std::string& name) {
        // std::cout<< 11 ;
        if (scope_.contains(name)) {
            return scope_[name];
        }
        // std::cout<< 22 << " " << name ;
        if (!parent_scope_) {
            throw NameError("");
        }
        // std::cout<< 33 ;
        return parent_scope_->GetVariableInScopes(name);
    }

    void SetVariable(const std::string& name, Object* value) {
        scope_[name] = value;
    }

    virtual void Dfs() override {
        mark_ = true;

        if (parent_scope_) {
            if (!parent_scope_->mark_) {
                parent_scope_->Dfs();
            }
        }

        for (auto [_, obj] : scope_) {
            if (!obj->mark_) {
                obj->Dfs();
            }
        }
    }

private:
    Scope* parent_scope_;
    std::unordered_map<std::string, Object*> scope_;
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
    Cell(Object* left, Object* right) : left_(left), right_(right) {
    }

    Object*& GetFirst() {
        return left_;
    }

    Object*& GetSecond() {
        return right_;
    }

    virtual void Dfs() override {
        mark_ = true;

        if (left_) {
            if (!left_->mark_) {
                left_->Dfs();
            }
        }

        if (right_) {
            if (!right_->mark_) {
                right_->Dfs();
            }
        }
    }

private:
    Object* left_;
    Object* right_;
};

static std::vector<Object*> CreateVectorFromList(Object* cur_node);
static Object* Eval(Object* cur_node);
static std::vector<Object*> EvalList(Object* cur_node);

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and convertion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
T* As(Object* obj) {
    return dynamic_cast<T*>(obj);
}

template <class T>
bool Is(Object* obj) {
    return dynamic_cast<T*>(obj) != nullptr;
}

// Heap function

inline Scope* current_scope = nullptr;

class Heap {
public:
    template <typename T, typename... Args>
    Object* Make(const Args&... args) {
        objects_.push_back((new T(args...)));
        return objects_.back();
    }

    inline Scope* Clone(Scope* clone) {
        Scope* scope = new Scope(*clone);
        objects_.push_back(scope);
        return scope;
    }

    void MarkAndSweep() {
        current_scope->Dfs();

        for (auto iter = objects_.begin(); iter != objects_.end();) {
            if (!(*iter)->mark_) {
                delete (*iter);
                iter = objects_.erase(iter);
            } else {
                (*iter)->mark_ = false;
                ++iter;
            }
        }
        // cout<< objects_.size() << " End Size\n";
    }

    std::vector<Object*> objects_;
};

inline Heap heap;

// Function

class Function : public Object {
public:
    virtual ~Function() = default;
    virtual Object* operator()(const std::vector<Object*>& params) = 0;
};

class NoEvalFunction : public Function {};

// Quote

class Quote : public Function {
public:
    virtual ~Quote() = default;
    virtual Object* operator()(const std::vector<Object*>& params) {
        return params[0];
    }
};

class QuoteFunction : public Quote {
public:
    virtual ~QuoteFunction() = default;
    virtual Object* operator()(const std::vector<Object*>& params) {
        if (params.size() != 1) {
            throw RuntimeError("");
        }
        return As<Cell>(params[0])->GetFirst();
    }
};

// IntegerFunctions

class IntegerFunction : public Function {
public:
    void Check(const std::vector<Object*>& params) {
        for (auto param : params) {
            if (!param || !Is<Number>(param)) {
                throw RuntimeError("");
            }
        }
    }
};

struct IsNumber : public IntegerFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {

        for (auto param : params) {
            if (!param || !Is<Number>(param)) {
                return heap.Make<Bool>(false);
            }
        }

        return heap.Make<Bool>(true);
    }
};

struct Sum : public IntegerFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        Check(params);

        int64_t ans = 0;
        for (auto param : params) {
            ans += As<Number>(param)->GetValue();
        }
        return heap.Make<Number>(ans);
    }
};

struct Difference : public IntegerFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        Check(params);

        if (params.empty()) {
            throw RuntimeError("");
        }

        int64_t ans = As<Number>(params[0])->GetValue();
        for (size_t i = 1; i < params.size(); ++i) {
            ans -= As<Number>(params[i])->GetValue();
        }
        return heap.Make<Number>(ans);
    }
};

struct Multiplication : public IntegerFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        Check(params);

        int64_t ans = 1;
        for (auto param : params) {
            ans *= As<Number>(param)->GetValue();
        }
        return heap.Make<Number>(ans);
    }
};

struct Division : public IntegerFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        Check(params);

        if (params.empty()) {
            throw RuntimeError("");
        }

        int64_t ans = As<Number>(params[0])->GetValue();
        for (size_t i = 1; i < params.size(); ++i) {
            ans /= As<Number>(params[i])->GetValue();
        }
        return heap.Make<Number>(ans);
    }
};

struct Equality : public IntegerFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        Check(params);
        for (size_t i = 1; i < params.size(); ++i) {
            if (As<Number>(params[i])->GetValue() != As<Number>(params[0])->GetValue()) {
                return heap.Make<Bool>(false);
            }
        }
        return heap.Make<Bool>(true);
    }
};

struct StrictlyDescending : public IntegerFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        Check(params);

        for (size_t i = 1; i < params.size(); ++i) {
            if (!(As<Number>(params[i - 1])->GetValue() > As<Number>(params[i])->GetValue())) {
                return heap.Make<Bool>(false);
            }
        }
        return heap.Make<Bool>(true);
    }
};

struct StrictlyAscending : public IntegerFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        Check(params);

        for (size_t i = 1; i < params.size(); ++i) {
            if (!(As<Number>(params[i - 1])->GetValue() < As<Number>(params[i])->GetValue())) {
                return heap.Make<Bool>(false);
            }
        }
        return heap.Make<Bool>(true);
    }
};

struct Descending : public IntegerFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        Check(params);

        for (size_t i = 1; i < params.size(); ++i) {
            if (!(As<Number>(params[i - 1])->GetValue() >= As<Number>(params[i])->GetValue())) {
                return heap.Make<Bool>(false);
            }
        }
        return heap.Make<Bool>(true);
    }
};

struct Ascending : public IntegerFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        Check(params);

        for (size_t i = 1; i < params.size(); ++i) {
            if (!(As<Number>(params[i - 1])->GetValue() <= As<Number>(params[i])->GetValue())) {
                return heap.Make<Bool>(false);
            }
        }
        return heap.Make<Bool>(true);
    }
};

struct Abs : public IntegerFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        if (params.size() != 1) {
            throw RuntimeError("");
        }

        Check(params);

        return heap.Make<Number>(std::abs(As<Number>(params.front())->GetValue()));
    }
};

struct Max : public IntegerFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        Check(params);

        if (params.empty()) {
            throw RuntimeError("");
        }

        int64_t ans = As<Number>(params[0])->GetValue();

        for (auto param : params) {
            ans = std::max(ans, As<Number>(param)->GetValue());
        }

        return heap.Make<Number>(ans);
    }
};

struct Min : public IntegerFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        Check(params);

        if (params.empty()) {
            throw RuntimeError("");
        }

        int64_t ans = As<Number>(params[0])->GetValue();

        for (auto param : params) {
            ans = std::min(ans, As<Number>(param)->GetValue());
        }

        return heap.Make<Number>(ans);
    }
};

// BoolFunctions

class BoolFunction : public NoEvalFunction {
public:
    Object* ParamToBool(Object* param) {
        param = Eval(param);
        return Is<Bool>(param) ? param : heap.Make<Bool>(true);
    }
};

class IsBool : public BoolFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        for (auto param : params) {
            if (!param || !Is<Bool>(param)) {
                return heap.Make<Bool>(false);
            }
        }
        return heap.Make<Bool>(true);
    }
};

class If : public BoolFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        if (params.size() == 2) {
            return As<Bool>(ParamToBool(params[0]))->GetValue() ? Eval(params[1]) : nullptr;
        }
        if (params.size() == 3) {
            return As<Bool>(ParamToBool(params[0]))->GetValue() ? Eval(params[1]) : Eval(params[2]);
        }
        throw SyntaxError("");
    }
};

class Or : public BoolFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        for (auto param : params) {
            if (As<Bool>(ParamToBool(param))->GetValue()) {
                return Eval(param);
            }
        }
        return heap.Make<Bool>(false);
    }
};

class And : public BoolFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        for (auto param : params) {
            if (!As<Bool>(ParamToBool(param))->GetValue()) {
                return heap.Make<Bool>(false);
            }
        }
        if (!params.empty()) {
            return Eval(params.back());
        }
        return heap.Make<Bool>(true);
    }
};

class Not : public BoolFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        if (params.size() != 1) {
            throw RuntimeError("");
        }
        return heap.Make<Bool>(!As<Bool>(ParamToBool(params[0]))->GetValue());
    }
};

// List Functions

class Car : public Function {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        if (params.empty() || !Is<Cell>(params[0]) || params.size() != 1) {
            throw RuntimeError("");
        }
        return As<Cell>(params[0])->GetFirst();
    }
};

class Cdr : public Function {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        if (params.empty() || !Is<Cell>(params[0]) || params.size() != 1) {
            throw RuntimeError("");
        }
        return As<Cell>(params[0])->GetSecond();
    }
};

class Cons : public Function {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        if (params.size() != 2) {
            throw RuntimeError("");
        }
        return heap.Make<Cell>(params[0], params[1]);
    }
};

class MakeList : public Function {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        if (params.empty()) {
            return nullptr;
        }
        return heap.Make<Cell>(params[0], MakeObjectFromList(params));
    }

private:
    Object* MakeObjectFromList(const std::vector<Object*>& params, size_t pos = 1) {
        if (pos == params.size()) {
            return nullptr;
        }
        return heap.Make<Cell>(params[pos], MakeObjectFromList(params, pos + 1));
    }
};

class IsList : public Function {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        if (params.size() != 1) {
            throw RuntimeError("");
        }
        if (!Is<Cell>(params[0]) && params[0]) {
            return heap.Make<Bool>(false);
        }
        return heap.Make<Bool>(Checker(params[0]));
    }

private:
    bool Checker(Object* param) {
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
    virtual Object* operator()(const std::vector<Object*>& params) override {
        if (params.size() != 1) {
            throw RuntimeError("");
        }
        return heap.Make<Bool>(CreateVectorFromList(params[0]).size() == 2);
    }
};

class IsNull : public Function {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        if (params.size() != 1) {
            throw RuntimeError("");
        }
        return heap.Make<Bool>(!params[0]);
    }
};

class GetListRef : public Function {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
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
    virtual Object* operator()(const std::vector<Object*>& params) override {
        if (params.size() != 2 || !Is<Cell>(params[0]) || !Is<Number>(params[1])) {
            throw SyntaxError("");
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

// Other

class IsSymbol : public Function {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        if (params.size() != 1) {
            throw RuntimeError("");
        }
        return heap.Make<Bool>(Is<Symbol>(params[0]));
    }
};

class LambdaFunction : public Function {
public:
    LambdaFunction(std::vector<Object*> args, std::vector<Object*> commands, Scope* parent)
        : args_(args), commands_(commands), scope_(As<Scope>(heap.Make<Scope>(parent))) {
    }

    virtual Object* operator()(const std::vector<Object*>& params) override {

        if (params.size() != args_.size()) {
            throw RuntimeError("");
        }

        Scope* new_scope = As<Scope>(heap.Make<Scope>(heap.Clone(scope_)));

        for (size_t i = 0; i < params.size(); ++i) {
            new_scope->SetVariable(As<Symbol>(args_[i])->GetName(), params[i]);
        }

        Scope* buf = current_scope;
        current_scope = new_scope;

        for (size_t i = 0; i < commands_.size() - 1; ++i) {
            Eval(commands_[i]);
        }

        auto return_object = Eval(commands_.back());
        current_scope = buf;

        return return_object;
    }

    virtual void Dfs() override {
        mark_ = true;

        for (auto i : args_) {
            if (i) {
                if (!i->mark_) {
                    i->Dfs();
                }
            }
        }

        for (auto i : commands_) {
            if (i) {
                if (!i->mark_) {
                    i->Dfs();
                }
            }
        }

        if (scope_) {
            if (!scope_->mark_) {
                scope_->Dfs();
            }
        }
    }

private:
    std::vector<Object*> args_;
    std::vector<Object*> commands_;
    Scope* scope_;
};

class Lambda : public NoEvalFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        if (params.size() < 2) {
            throw SyntaxError("");
        }
        if (params[0] && !Is<Cell>(params[0])) {
            throw RuntimeError("");
        }

        auto lambda_args = CreateVectorFromList(params[0]);
        std::vector<Object*> commands;
        commands.resize(params.size() - 1);
        std::copy(std::next(params.begin()), params.end(), commands.begin());
        return heap.Make<LambdaFunction>(lambda_args, commands, current_scope);
    }
};

class Define : public NoEvalFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        if (params.size() < 2) {
            throw SyntaxError("");
        }
        if (Is<Cell>(params[0])) {
            std::vector<Object*> copy = params;
            copy[0] = As<Cell>(copy[0])->GetSecond();
            current_scope->SetVariable(As<Symbol>(As<Cell>(params[0])->GetFirst())->GetName(),
                                       Lambda()(copy));
        } else {
            if (params.size() != 2) {
                throw SyntaxError("");
            }
            if (!Is<Symbol>(params[0])) {
                throw RuntimeError("");
            }
            current_scope->SetVariable(As<Symbol>(params[0])->GetName(), Eval(params[1]));
        }
        return nullptr;
    }
};

class Set : public NoEvalFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        if (params.size() != 2) {
            throw SyntaxError("");
        }
        if (!Is<Symbol>(params[0])) {
            throw RuntimeError("");
        }
        current_scope->GetVariableInScopes(As<Symbol>(params[0])->GetName()) = Eval(params[1]);
        return heap.Make<Symbol>("");
    }
};

class SetCar : public NoEvalFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        if (params.size() != 2) {
            throw RuntimeError("");
        }
        if (Is<Symbol>(params[0])) {
            auto& obj = current_scope->GetVariableInScopes(As<Symbol>(params[0])->GetName());
            if (!Is<Cell>(obj)) {
                throw RuntimeError("");
            }
            As<Cell>(obj)->GetFirst() = Eval(params[1]);
        } else {
            if (!Is<Cell>(params[0])) {
                throw RuntimeError("");
            }
            As<Cell>(Eval(params[0]))->GetFirst() = Eval(params[1]);
        }
        return heap.Make<Symbol>("");
    }
};

class SetCdr : public NoEvalFunction {
public:
    virtual Object* operator()(const std::vector<Object*>& params) override {
        if (params.size() != 2) {
            throw RuntimeError("");
        }
        if (Is<Symbol>(params[0])) {
            auto& obj = current_scope->GetVariableInScopes(As<Symbol>(params[0])->GetName());
            if (!Is<Cell>(obj)) {
                throw RuntimeError("");
            }
            As<Cell>(obj)->GetSecond() = Eval(params[1]);
        } else {
            if (!Is<Cell>(params[0])) {
                throw RuntimeError("");
            }
            As<Cell>(Eval(params[0]))->GetSecond() = Eval(params[1]);
        }
        return heap.Make<Symbol>("");
    }
};

static Scope global = Scope(nullptr, {// Quote
                                      {"'", heap.Make<Quote>()},
                                      {"quote", heap.Make<QuoteFunction>()},

                                      // IntegerFunctions
                                      {"+", heap.Make<Sum>()},
                                      {"-", heap.Make<Difference>()},
                                      {"*", heap.Make<Multiplication>()},
                                      {"/", heap.Make<Division>()},
                                      {"=", heap.Make<Equality>()},
                                      {"<", heap.Make<StrictlyAscending>()},
                                      {">", heap.Make<StrictlyDescending>()},
                                      {"<=", heap.Make<Ascending>()},
                                      {">=", heap.Make<Descending>()},
                                      {"abs", heap.Make<Abs>()},
                                      {"min", heap.Make<Min>()},
                                      {"max", heap.Make<Max>()},
                                      {"number?", heap.Make<IsNumber>()},

                                      // BooleFunctions
                                      {"if", heap.Make<If>()},
                                      {"or", heap.Make<Or>()},
                                      {"and", heap.Make<And>()},
                                      {"not", heap.Make<Not>()},
                                      {"boolean?", heap.Make<IsBool>()},

                                      // ListFunctions
                                      {"car", heap.Make<Car>()},
                                      {"cdr", heap.Make<Cdr>()},
                                      {"cons", heap.Make<Cons>()},
                                      {"list", heap.Make<MakeList>()},
                                      {"list?", heap.Make<IsList>()},
                                      {"pair?", heap.Make<IsPair>()},
                                      {"null?", heap.Make<IsNull>()},
                                      {"list-ref", heap.Make<GetListRef>()},
                                      {"list-tail", heap.Make<GetListTail>()},

                                      // Other
                                      {"symbol?", heap.Make<IsSymbol>()},
                                      {"define", heap.Make<Define>()},
                                      {"set!", heap.Make<Set>()},
                                      {"set-car!", heap.Make<SetCar>()},
                                      {"set-cdr!", heap.Make<SetCdr>()},
                                      {"lambda", heap.Make<Lambda>()}});

static std::vector<Object*> CreateVectorFromList(Object* cur_node) {
    if (!cur_node) {
        return {};
    }
    if (!Is<Cell>(cur_node)) {
        return {cur_node};
    }
    std::vector<Object*> vec1 = {As<Cell>(cur_node)->GetFirst()};
    std::vector<Object*> vec2 = CreateVectorFromList(As<Cell>(cur_node)->GetSecond());
    for (auto obj : vec2) {
        vec1.push_back(obj);
    }
    return vec1;
}

static std::vector<Object*> EvalList(Object* cur_node) {
    if (!cur_node) {
        return {};
    }
    if (!Is<Cell>(cur_node)) {
        return {Eval(cur_node)};
    }
    std::vector<Object*> vec1 = {Eval(As<Cell>(cur_node)->GetFirst())};
    std::vector<Object*> vec2 = EvalList(As<Cell>(cur_node)->GetSecond());
    for (auto obj : vec2) {
        vec1.push_back(obj);
    }
    return vec1;
}

static Object* Eval(Object* cur_node) {
    // std::cout<< 1 ;
    if (!cur_node) {
        throw RuntimeError("");
    }
    // std::cout<< 2 ;
    if (Is<Number>(cur_node) || Is<Bool>(cur_node)) {
        return cur_node;
    }
    // std::cout<< 3 ;
    if (Is<Symbol>(cur_node)) {
        return current_scope->GetVariableInScopes(As<Symbol>(cur_node)->GetName());
    }
    // std::cout<< 4 ;
    // Cell
    auto cell = As<Cell>(cur_node);
    auto calculated_function = Eval(cell->GetFirst());
    // std::cout<< 5 ;
    if (!Is<Function>(calculated_function)) {
        throw RuntimeError("");
    }
    // std::cout<< 6 ;
    if (Is<Quote>(calculated_function)) {
        return (*As<Quote>(calculated_function))({cell->GetSecond()});
    }
    // std::cout<< 7 ;
    if (Is<NoEvalFunction>(calculated_function)) {
        return (*As<NoEvalFunction>(calculated_function))(CreateVectorFromList(cell->GetSecond()));
    }
    // std::cout<< 8 ;
    return (*As<Function>(calculated_function))(EvalList(cell->GetSecond()));
}
