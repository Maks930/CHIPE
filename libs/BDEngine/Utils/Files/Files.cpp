//
// Created by Maks930 on 08/25/2025.
//

#include "Files.h"

#include <fstream>
#include <sstream>

namespace bde::utils::files {
    std::string readFile(const std::string &path) {
        std::string content;
        std::ifstream file;

        file.exceptions(std::ifstream::badbit);

        try {
            file.open(path);
            std::stringstream ss;
            ss << file.rdbuf();
            file.close();
            content = ss.str();
        } catch (const std::ifstream::failure &e) {
            return "";
        }

        return content;
    }
};