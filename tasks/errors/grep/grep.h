#pragma once

#include <algorithm>
#include <iterator>
#include <optional>
#include <string>
#include <fstream>
#include <functional>
#include <iostream>
#include <filesystem>
#include <vector>
#include "utf8/core.h"
#include "utf8/unchecked.h"
#include "utf8/checked.h"

using std::optional;
namespace fs = std::filesystem;

struct GrepOptions {
    optional<size_t> look_ahead_length;
    size_t max_matches_per_line;

    GrepOptions() {
        max_matches_per_line = 10;
    }

    GrepOptions(size_t look_ahead_length) : GrepOptions() {
        this->look_ahead_length = look_ahead_length;
    }

    GrepOptions(optional<size_t> look_ahead_length, size_t max_matches_per_line) {
        this->look_ahead_length = look_ahead_length;
        this->max_matches_per_line = max_matches_per_line;
    }
};

template <class Visitor>
void Grep(const std::string& path, const std::string& pattern, Visitor visitor,
          const GrepOptions& options) {

    if (!fs::is_directory(path)) {
        if (fs::is_symlink(path)) {
            return;
        }
        std::ifstream fs8(path);
        if (!fs8.is_open()) {
            return;
        }

        std::vector<unsigned short> pattern16;
        utf8::utf8to16(pattern.begin(), pattern.end(), back_inserter(pattern16));
        
        std::string utf8line;
        size_t line_num = 1;

        while (getline(fs8, utf8line)) {
            if (utf8::find_invalid(utf8line.begin(), utf8line.end()) != utf8line.end()) {
                visitor.OnError("Invalid utf8 at " + path);
                return;
            }

            std::vector<unsigned short> line;
            utf8::utf8to16(utf8line.begin(), utf8line.end(), back_inserter(line));

            auto search = [&](std::vector<unsigned short>::iterator begin) {
                return std::search(begin, line.end(), std::boyer_moore_searcher(pattern16.begin(), pattern16.end()));
            };
            
            int  lines_matches = 0;
            for (auto it = search(line.begin()); it != line.end() && lines_matches != options.max_matches_per_line; ++lines_matches, it = search(++it)) {
                optional<std::string> opt = std::nullopt;
                if (options.look_ahead_length) {
                    opt = "";
                    utf8::utf16to8(it + pattern16.size(), std::min(line.end(), it + pattern16.size() + options.look_ahead_length.value()), back_inserter(opt.value()));
                } 
                visitor.OnMatch(path, line_num, std::distance(line.begin(), it) + 1, opt);
            }
            ++line_num;
        }
        fs8.close();
    } else {
        for (const auto& dirEntry : fs::recursive_directory_iterator(path)) {
            if (!fs::is_directory(dirEntry)) {
                Grep(dirEntry.path(), pattern, visitor, options);
            }
        }
    }
}
