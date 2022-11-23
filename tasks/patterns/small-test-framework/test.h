#pragma once

#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class AbstractTest {
public:
    virtual void SetUp() = 0;
    virtual void TearDown() = 0;
    virtual void Run() = 0;
    virtual ~AbstractTest() {
    }
};

std::function<bool(std::string)> FullMatch(std::string str) {
    return [orig = std::move(str)](const std::string& str) { return str == orig; };
}

std::function<bool(std::string)> Substr(std::string str) {
    return [substr = std::move(str)](const std::string& str) {
        return str.find(substr) != std::basic_string<char>::npos;
    };
}

class TestRegistry {
public:
    template <class TestClass>
    void RegisterClass(const std::string& class_name) {
        factory_map_[class_name] = []() { return std::make_unique<TestClass>(); };
    }

    static TestRegistry& Instance() {
        static TestRegistry instance;
        return instance;
    }

    std::unique_ptr<AbstractTest> CreateTest(const std::string& class_name) {
        if (!factory_map_.contains(class_name)) {
            throw std::out_of_range("Name doesn't exist");
        }
        return factory_map_[class_name]();
    }

    void RunTest(const std::string& test_name) {
        auto test = factory_map_[test_name]();
        test->SetUp();
        try {
            test->Run();
        } catch (...) {
            test->TearDown();
            throw;
        }
        test->TearDown();
    }

    template <class Predicate>
    std::vector<std::string> ShowTests(Predicate callback) const {
        std::vector<std::string> tests;
        for (auto [name, _] : factory_map_) {
            if (callback(name)) {
                tests.push_back(name);
            }
        }
        return tests;
    }

    std::vector<std::string> ShowAllTests() const {
        std::vector<std::string> tests;
        for (auto [name, _] : factory_map_) {
            tests.push_back(name);
        }
        return tests;
    }

    template <class Predicate>
    void RunTests(Predicate callback) {
        for (auto [name, _] : factory_map_) {
            if (callback(name)) {
                RunTest(name);
            }
        }
    }

    void Clear() {
        factory_map_.clear();
    }

private:
    std::map<std::string, std::function<std::unique_ptr<AbstractTest>()>> factory_map_;
};
