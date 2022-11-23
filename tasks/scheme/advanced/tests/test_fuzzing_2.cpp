#include <scheme.h>

#include "scheme_test.h"
#include <fuzzer.h>

#include <iostream>

static constexpr uint32_t kShotsCount = 100000;

TEST_CASE("Fuzzing-2") {
    Fuzzer fuzzer;
    Interpreter interpreter;

    for (uint32_t i = 0; i < kShotsCount; ++i) {
        try {
            auto req = fuzzer.Next();
            // std::cerr << req << std::endl;  // uncomment to debug, random is deterministic
            interpreter.Run(req);
        } catch (const SyntaxError&) {
        } catch (const RuntimeError&) {
        } catch (const NameError&) {
        }
    }
}
