//
// Created by Maks930 on 15/11/2025.
//

#include "Preprocessor.h"
#include <algorithm>
#include <ranges>
#include <vector>
#include <iostream>

constexpr char comment_sym = ';';
constexpr char directive_sym = '%';

std::string ltrim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\n\r\f\v");
    return (start == std::string::npos) ? "" : s.substr(start);
}


std::string rtrim(const std::string& s) {
    size_t end = s.find_last_not_of(" \t\n\r\f\v");
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}


std::string trim(const std::string& s) {
    return rtrim(ltrim(s));
}

std::string Preprocessor::preprocess(const std::string &str) {
    std::string res = "";

    return res;
}

std::string Preprocessor::preprocess_line(const std::string &line) {
    auto res = trim(line);
    if (res.at(0) == comment_sym) {
        return "";
    }

    if (res.at(0) == directive_sym) {
        res = work_directive(res);
    }

    return res;
}

std::string Preprocessor::work_directive(const std::string &line) {

}
