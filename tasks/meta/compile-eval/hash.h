#pragma once

#include <cstdint>

constexpr int hash2(const char *s, int64_t p_cur, int64_t p_orig, int64_t mod) {
    return (*s != '\0') ? (((*s * p_cur) % mod + hash2(s + 1, (p_cur * p_orig) % mod, p_orig, mod)) % mod) : 0;
}

constexpr int hash(const char *s, int64_t p, int64_t mod) {
    return (*s != '\0') ? ((*s % mod + hash2(s + 1, p % mod, p % mod, mod)) % mod) : 0;
}
