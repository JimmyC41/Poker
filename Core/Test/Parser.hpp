#pragma once
#include <gtest/gtest.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>

namespace Parser {

namespace fs = std::filesystem;

struct Line;

using VecStr = std::vector<std::string>;
using VecLine = std::vector<Line>;

struct Line {
    std::string m_operation;
    VecStr m_args;
    int m_line = 0;
};

inline VecStr tokenize_str(const std::string& line) {
    std::istringstream iss(line);
    VecStr out;
    std::string w;
    while (iss >> w) out.emplace_back(w);
    return out;
}

inline VecLine load_file(const fs::path& p) {
    std::ifstream in(p);
    if (!in)
        throw std::runtime_error("Cannot open file " + p.string());
    
    VecLine all_lines;
    std::string line_str;
    for (int line_no = 1; std::getline(in, line_str); ++line_no) {
        auto first = line_str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) continue;
        auto last = line_str.find_last_not_of(" \t\r\n");
        auto trimmed = line_str.substr(first, last - first + 1);
        auto tokens = tokenize_str(trimmed);
        if (tokens.empty()) continue;
        Line line;
        line.m_operation = tokens.front();
        tokens.erase(tokens.begin());
        line.m_args = std::move(tokens);
        line.m_line = line_no;
        all_lines.emplace_back(line);
    }
    return all_lines;
}

inline VecStr list_files(const fs::path& dir, const char* ext = ".txt") {
    VecStr files;
    if (fs::exists(dir)) {
        for (auto& e : fs::directory_iterator(dir)) {
            if (e.is_regular_file() && e.path().extension() == ext)
                files.push_back(e.path().string());
        }
        std::sort(files.begin(), files.end());
    }
    return files;
}

static inline Poker::PlayerID as_id(const std::string& s) {
    return static_cast<Poker::PlayerID>(std::stoll(s));
}

static inline Poker::Chips as_chips(const std::string& s) {
    return Poker::Chips{static_cast<std::int64_t>(std::stoll(s))};
}

static inline bool as_bool(const std::string& s) {
    return std::stoi(s) != 0;
}


template<class T>
class FixtureBase : public ::testing::Test {
protected:
    virtual void on_step(const Line& l, T& t) = 0;
    virtual void on_expect(const Line& l, T& t) = 0;

    void run_script(const VecLine& lines, T& t) {
        for (const auto& l : lines) {
            SCOPED_TRACE(testing::Message() << "line" << l.m_line << ": " << l.m_operation);
            if (l.m_operation == "EXPECT")
                on_expect(l, t);
            else
                on_step(l, t);
        }
    }
};

}