#include <gtest/gtest.h>
#include <fmt/base.h>

#include <Lexer.h>
#include <variant>



TEST(Preprocessor, Case_Define) {

    std::string_view in_data = "jp _start\n_procedure:\nsub V1, V2\nret\n\n_start:\nld V1, 0xFF\nld V2, 1\ncall _procedure\nhlt\njp _start + 4";

    Lexer lexer;

    auto processed = lexer.processed(in_data);

    fmt::print("Program\n");
    for (auto& i : processed) {
        if (const auto e = std::get_if<Lexer::Identifier>(&i)) {
            fmt::print("Identifier {}\n", e->idf);
        } else if (const auto e = std::get_if<Lexer::Mark>(&i)) {
            fmt::print("Mark {}\n", e->idf);
        }
        else if (const auto e = std::get_if<Lexer::Integer>(&i)) {
            fmt::print("Integer {}\n", e->value);
        }
        else if (const auto e = std::get_if<Lexer::Keyword>(&i)) {
            std::string s = "";

            for (auto [key, value] : Lexer::keywords) {
                if ((int)value == (int)(*e)) {
                    s = key;
                }
            }

            fmt::print("Keyword: {}\n", s);
        }
        else if (const auto e = std::get_if<Lexer::Register>(&i)) {
            std::string s = "";

            for (auto [key, value] : Lexer::registers) {
                if ((int)value == (int)(*e)) {
                    s = key;
                }
            }

            fmt::print("Register: {}\n", s);
        }
        else if (const auto e = std::get_if<Lexer::Plus>(&i)) {
            fmt::print("PLUS\n");
        }
        else if (const auto e = std::get_if<Lexer::Sub>(&i)) {
            fmt::print("SUB\n");
        }
        else if (const auto e = std::get_if<Lexer::LSQPar>(&i)) {
            fmt::print("LSQPar\n");
        }
        else if (const auto e = std::get_if<Lexer::RSQPar>(&i)) {
            fmt::print("RSQPar\n");
        }

    }fmt::print("Program\n");

    // 0x1206 0x8125 0x00EE 0x61FF 0x6201 0x2202 0x0000 0x120A
    std::vector<Lexer::Token> expected{
        Lexer::Keyword::JP,
        Lexer::Identifier{.idf="_start"},
        //Lexer::Keyword::NL,
        Lexer::Mark{.idf="_procedure"},
        //Lexer::Keyword::NL,
        Lexer::Keyword::SUB,
        Lexer::Register::V1,
        Lexer::Register::V2,
        //Lexer::Keyword::NL,
        Lexer::Keyword::RET,
        //Lexer::Keyword::NL,
        Lexer::Mark{.idf = "_start"},
        //Lexer::Keyword::NL,
        Lexer::Keyword::LD,
        Lexer::Register::V1,
        Lexer::Integer{.value=0xFF},
        //Lexer::Keyword::NL,
        Lexer::Keyword::LD,
        Lexer::Register::V2,
        Lexer::Integer{.value = 1},
        //Lexer::Keyword::NL,
        Lexer::Keyword::CALL,
        Lexer::Identifier{.idf="_procedure"},
        //Lexer::Keyword::NL,
        Lexer::Keyword::HLT,
        //Lexer::Keyword::NL,
        Lexer::Keyword::JP,
        Lexer::Identifier{.idf = "_start"},
        Lexer::Plus{},
        Lexer::Integer{.value=4},
        //Lexer::Keyword::NL
    };
    

    

    EXPECT_EQ(expected, processed);
}