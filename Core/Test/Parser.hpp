#pragma once
#include <gtest/gtest.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include "Pot.hpp"
#include "Chips.hpp"

namespace Poker {

namespace fs = std::filesystem;
using Poker::PlayerID;
using Poker::Chips;

struct Line {
    std::string operation;
    std::vector<std::string> args;
    int line_no = 0;
};

inline std::vector<std::string> tokenize_str(const std::string& line) {
    std::istringstream iss(line);
    std::vector<std::string> out;
    std::string w;
    while (iss >> w) out.emplace_back(w);
    return out;
}

inline std::vector<Line> load_file(const fs::path& p) {
    std::ifstream in(p);
    if (!in)
        throw std::runtime_error("Cannot open file " + p.string());
    
    std::vector<Line> all_lines;
    std::string line_str;
    for (int line_no = 1; std::getline(in, line_str); ++line_no) {
        auto first = line_str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) continue;
        auto last = line_str.find_last_not_of(" \t\r\n");
        auto trimmed = line_str.substr(first, last - first + 1);
        auto tokens = tokenize_str(trimmed);
        if (tokens.empty()) continue;
        Line line;
        line.operation = tokens.front();
        tokens.erase(tokens.begin());
        line.args = std::move(tokens);
        line.line_no = line_no;
        all_lines.emplace_back(line);
    }
    return all_lines;
}

inline std::vector<std::string> list_files(const fs::path& dir, const char* ext = ".txt") {
    std::vector<std::string> files;
    if (fs::exists(dir)) {
        for (auto& e : fs::directory_iterator(dir)) {
            if (e.is_regular_file() && e.path().extension() == ext)
                files.push_back(e.path().string());
        }
        std::sort(files.begin(), files.end());
    }
    return files;
}

static inline PlayerID as_id(const std::string& s) {
    return static_cast<PlayerID>(std::stoll(s));
}

static inline Chips as_chips(const std::string& s) {
    return Chips{static_cast<std::int64_t>(std::stoll(s))};
}

static inline bool as_bool(const std::string& s) {
    return std::stoi(s) != 0;
}

template<class T>
class FixtureBase : public ::testing::Test {
protected:
    virtual void on_expect(const Line& l, T& t) = 0;
    virtual void on_step(const Line& l, T& t) = 0;

    void run_script(const std::vector<Line>& lines, T& t) {
        for (const auto& l : lines) {
            SCOPED_TRACE(testing::Message() << "line" << l.line_no << ": " << l.operation);
            if (l.operation == "EXPECT")
                on_expect(l, t);
            else
                on_step(l, t);
        }
    }
};

} // namespace Poker