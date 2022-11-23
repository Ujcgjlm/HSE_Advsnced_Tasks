#include "string_operations.h"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <string>
#include <string_view>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

bool StartsWith(std::string_view string, std::string_view text) {
    return string.find(text) == 0;
}

bool EndsWith(std::string_view string, std::string_view text) {
    return string.find(text) == (string.size() - text.size());
}

std::string_view StripPrefix(std::string_view string, std::string_view prefix) {
    if (StartsWith(string, prefix)) {
        string.remove_prefix(prefix.size());
    }
    return string;
}

std::string_view StripSuffix(std::string_view string, std::string_view suffix) {
    if (EndsWith(string, suffix)) {
        string.remove_suffix(suffix.size());
    }
    return string;
}

std::string_view ClippedSubstr(std::string_view s, size_t pos, size_t n) {
    if (pos + n > s.size()) {
        return s;
    } else {
        return std::string_view(s.begin() + pos, s.begin() + pos + n);
    }
}

std::string_view StripAsciiWhitespace(std::string_view s) {
    while (!s.empty()) {
        if (std::isspace(s.front())) {
            s.remove_prefix(1);
        } else {
            break;
        }
    }

    while (!s.empty()) {
        if (std::isspace(s.back())) {
            s.remove_suffix(1);
        } else {
            break;
        }
    }

    return s;
}

std::vector<std::string_view> StrSplit(std::string_view text, std::string_view delim) {
    size_t size = 0;
    size_t prevpos = 0;
    for (size_t pos = text.find(delim); pos != std::string_view::npos;
         prevpos = pos, pos = text.find(delim, pos + delim.size())) {
        ++size;
    }
    std::vector<std::string_view> ans;
    ans.reserve(size + 1);

    prevpos = 0;
    for (size_t pos = text.find(delim); pos != std::string_view::npos;
         prevpos = pos + delim.size(), pos = text.find(delim, pos + delim.size())) {
        ans.emplace_back(std::string_view(text.begin() + prevpos, text.begin() + pos));
    }

    ans.emplace_back(std::string_view(text.begin() + prevpos, text.end()));
    return ans;
}

std::string ReadN(const std::string& filename, size_t n) {
    int pfd;
    if ((pfd = open(filename.c_str(), O_RDONLY)) == -1) {
        return "";
    }

    std::string s;
    s.resize(n);
    read(pfd, s.data(), n);
    return s;
}

std::string AddSlash(std::string_view path) {
    if (!path.empty() && path.back() != '/') {
        std::string s;
        s.reserve(path.size() + 1);
        s += path;
        s += "/";
        return s;
    }
    return std::string(path);
}

std::string_view RemoveSlash(std::string_view path) {
    if (!path.empty() && path.back() == '/' && path.size() != 1) {
        path.remove_suffix(1);
        return path;
    }
    return path;
}

std::string_view Dirname(std::string_view path) {
    while (path.back() != '/') {
        path.remove_suffix(1);
    }

    if (path == "/") {
        return path;
    }

    path.remove_suffix(1);

    return path;
}

std::string_view Basename(std::string_view path) {
    if (path.back() == '/') {
        path.remove_suffix(1);
    }

    while (path.find('/') != std::string_view::npos) {
        path.remove_prefix(1);
    }

    return path;
}

std::string CollapseSlashes(std::string_view path) {
    size_t ans_size = 0;
    bool is_prev_slash = false;

    for (auto chr : path) {
        if (!(chr == '/' && is_prev_slash)) {
            if (chr == '/') {
                is_prev_slash = true;
            } else {
                is_prev_slash = false;
            }
            ++ans_size;
        }
    }

    std::string ans;
    ans.reserve(ans_size);

    size_t pos = 0;
    is_prev_slash = false;

    for (auto chr : path) {
        if (!(chr == '/' && is_prev_slash)) {
            if (chr == '/') {
                is_prev_slash = true;
            } else {
                is_prev_slash = false;
            }
            ans += chr;
            ++pos;
        }
    }

    return ans;
}

std::string StrJoin(const std::vector<std::string_view>& strings, std::string_view delimiter) {
    if (strings.empty()) {
        return std::string();
    }

    size_t ans_size = (strings.size() - 1) * delimiter.size();

    for (auto str : strings) {
        ans_size += str.size();
    }

    std::string ans;
    ans.reserve(ans_size);
    size_t cur_i = 0;

    ans += strings[0];

    for (auto i = strings.begin() + 1; i != strings.end(); ++i) {
        auto str = *i;
        for (char chr : delimiter) {
            ans += chr;
            ++cur_i;
        }

        for (char chr : str) {
            ans += chr;
            ++cur_i;
        }
    }

    return ans;
}
