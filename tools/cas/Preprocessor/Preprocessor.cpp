#include "Preprocessor.h"
#include <algorithm>
#include <ranges>
#include <vector>
#include <iostream>
#include <string>

#include <utils.h>

constexpr char comment_sym = ';';



std::string Preprocessor::preprocess(const std::string &str) {
    std::string res = "";
    std::string source = str+"\n";

    auto pos = str.find('\n');
    while (pos != source.npos) {
                
        std::string line = source.substr(0,pos);

        source.erase(source.begin(), source.begin() + pos+1);

        line = preprocess_line(line);
        if (!line.empty()) {
            trim(line);
            res += line + "\n";
        }

        pos = source.find('\n');
    }
    res.pop_back();
    std::cout << res << std::endl;

    return res;
}

std::string Preprocessor::preprocess_line(const std::string &line) {
    std::string res = line;
    if (res.starts_with(comment_sym)) {
        return "";
    }

    auto pos = res.find_first_of(comment_sym, 0);
    if (pos != res.npos) {
        res.erase(res.begin()+pos, res.end());
    }
    return res;
}

std::string Preprocessor::work_directive(const std::string &line) {
    return "";
}
