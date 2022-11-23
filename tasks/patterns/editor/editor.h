#pragma once

#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>

namespace {

class TCommand {
public:
    virtual bool operator()(std::string& str, std::string::iterator& iter) {
        throw std::logic_error("Abstract type");
    }
    virtual void Undo(std::string& str, std::string::iterator& iter) {
        throw std::logic_error("Abstract type");
    }
};

class TShiftLeft : public TCommand {
public:
    bool operator()(std::string& str, std::string::iterator& iter) override {
        if (iter != str.begin()) {
            --iter;
            return true;
        }
        return false;
    }

    void Undo(std::string& str, std::string::iterator& iter) override {
        if (iter != str.end()) {
            ++iter;
        }
    }
};

class TShiftRight : public TCommand {
public:
    bool operator()(std::string& str, std::string::iterator& iter) override {
        if (iter != str.end()) {
            ++iter;
            return true;
        }
        return false;
    }

    void Undo(std::string& str, std::string::iterator& iter) override {
        if (iter != str.begin()) {
            --iter;
        }
    }
};

class TType : public TCommand {
public:
    TType(char chr) : chr_(chr) {
    }

    bool operator()(std::string& str, std::string::iterator& iter) override {
        iter = str.insert(iter, chr_);
        ++iter;
        return true;
    }

    void Undo(std::string& str, std::string::iterator& iter) override {
        --iter;
        iter = str.erase(iter);
    }

private:
    char chr_ = 0;
};

class TBackspace : public TCommand {
public:
    bool operator()(std::string& str, std::string::iterator& iter) override {
        if (iter == str.begin()) {
            return false;
        }
        --iter;
        chr_ = *iter;
        iter = str.erase(iter);
        return true;
    }

    void Undo(std::string& str, std::string::iterator& iter) override {
        str.insert(iter, chr_);
        ++iter;
    }

private:
    char chr_ = 0;
};

}  // namespace

class Editor {
public:
    const std::string& GetText() const {
        return str_;
    }

    void Type(char c) {
        DoCommand(std::make_unique<TType>(c));
    }

    void ShiftLeft() {
        DoCommand(std::make_unique<TShiftLeft>());
    }

    void ShiftRight() {
        DoCommand(std::make_unique<TShiftRight>());
    }

    void Backspace() {
        DoCommand(std::make_unique<TBackspace>());
    }

    void Undo() {
        if (history_cursor_ != history_.begin()) {
            --history_cursor_;
            (*history_cursor_)->Undo(str_, str_cursor_);
        }
    }

    void Redo() {
        if (history_cursor_ != history_.end()) {
            (*history_cursor_)->operator()(str_, str_cursor_);
            ++history_cursor_;
        }
    }

private:
    std::string str_;
    std::string::iterator str_cursor_ = str_.begin();

    std::list<std::unique_ptr<TCommand>> history_;
    std::list<std::unique_ptr<TCommand>>::iterator history_cursor_ = history_.begin();

    void DoCommand(std::unique_ptr<TCommand> cmd) {
        if ((*cmd)(str_, str_cursor_)) {
            history_.erase(history_cursor_, history_.end());
            history_.push_back(std::move(cmd));
            history_cursor_ = history_.end();
        }
    }
};
