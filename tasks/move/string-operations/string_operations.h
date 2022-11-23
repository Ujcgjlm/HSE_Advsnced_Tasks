#pragma once

#include <algorithm>
#include <cstring>
#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

bool StartsWith(std::string_view string, std::string_view text);
bool EndsWith(std::string_view string, std::string_view text);
std::string_view StripPrefix(std::string_view string, std::string_view prefix);
std::string_view StripSuffix(std::string_view string, std::string_view suffix);
std::string_view ClippedSubstr(std::string_view s, size_t pos, size_t n = std::string_view::npos);
std::string_view StripAsciiWhitespace(std::string_view s);
std::vector<std::string_view> StrSplit(std::string_view text, std::string_view delim);
std::string ReadN(const std::string& filename, size_t n);
std::string AddSlash(std::string_view path);
std::string_view RemoveSlash(std::string_view path);
std::string_view Dirname(std::string_view path);
std::string_view Basename(std::string_view path);
std::string CollapseSlashes(std::string_view path);
std::string StrJoin(const std::vector<std::string_view>& strings, std::string_view delimiter);

template <typename T>
size_t CountChars(const T& a) {
    if constexpr (std::is_same_v<std::decay_t<T>, std::string> ||
                  std::is_same_v<std::decay_t<T>, std::string_view> ||
                  std::is_same_v<std::decay_t<T>, char*> ||
                  std::is_same_v<std::decay_t<T>, const char*>) {
        return std::string_view(a).size();
    } else {
        auto b = a;
        size_t ans = (b < 0ll);
        if (!b) {
            return 1;
        }
        while (b) {
            ++ans;
            b /= 10ll;
        }
        return ans;
    }
}

template <typename T>
void FillStringByValues(const T& a, std::string& s) {
    if constexpr (std::is_same_v<std::decay_t<T>, std::string> ||
                  std::is_same_v<std::decay_t<T>, std::string_view> ||
                  std::is_same_v<std::decay_t<T>, char*> ||
                  std::is_same_v<std::decay_t<T>, const char*>) {
        s += a;
    } else {
        auto b = a;
        size_t size = s.size();
        if (!b) {
            s += '0';
            return;
        }
        if (b < 0ll) {
            s += '-';
            ++size;
            while (b) {
                s += -1 * (b % 10ll) + '0';
                b /= 10ll;
            }
        } else {
            while (b) {
                s += b % 10ll + '0';
                b /= 10ll;
            }
        }
        std::reverse(s.begin() + size, s.end());
    }
}

template <typename... Args>
std::string StrCat(Args&&... values) {
    size_t chars_count = (0 + ... + CountChars(values));
    std::string s;
    s.reserve(chars_count);
    (..., FillStringByValues(values, s));
    return s;
}
