#pragma once

#include <cstring>
#include <memory>
#include <string>

class StringView {
public:
    StringView(const std::string& str, const size_t front_char_pos = 0,
               const size_t back_char_pos = -1)
        : front_char_(&str[front_char_pos]),
          back_char_(&str[std::min(std::min(back_char_pos, str.size()) + front_char_pos - 1,
                                   str.size() - 1)]),
          size_(std::min(std::min(back_char_pos, str.size()) + front_char_pos - 1, str.size() - 1) -
                front_char_pos + 1) {
    }

    StringView(const char* str, const size_t back_char_pos = -1)
        : front_char_(str),
          back_char_(str + std::min(back_char_pos, std::strlen(str)) - 1),
          size_(std::min(back_char_pos, std::strlen(str))) {
    }

    char operator[](size_t pos) const {
        return *(front_char_ + pos);
    }

    size_t Size() const {
        return size_;
    }

private:
    const char* front_char_;
    const char* back_char_;

    size_t size_;
};
