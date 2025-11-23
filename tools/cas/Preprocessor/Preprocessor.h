//
// Created by Maks930 on 15/11/2025.
//

#ifndef CHIPE_PREPROCESSOR_H
#define CHIPE_PREPROCESSOR_H

#include <string>

class Preprocessor {
public:
    static std::string preprocess(const std::string & str);
    static std::string preprocess_line(const std::string& line);

private:
    static std::string work_directive(const std::string & line);
};


#endif //CHIPE_PREPROCESSOR_H