#include <gtest/gtest.h>
#include <fmt/base.h>

#include <Lexer.h>
#include <variant>



TEST(Preprocessor, Case_Define) {

    std::string_view in_data = "_procedure:\nmov V2, V1\nadd V2, 5\nret\n\n_start:\nld V1, 0xEE\nsub V1, 3\ncall _procedure\njp _start + 2";

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

    std::vector<Lexer::Token> expected{
        Lexer::Mark{.idf = "_procedure"},
        Lexer::Keyword::LD,
        Lexer::Keyword::V2,
        Lexer::Keyword::V1,
        Lexer::Keyword::ADD,
        Lexer::Keyword::V2,
        Lexer::Integer{.value = 5},
        Lexer::Keyword::RET,
        Lexer::Mark{.idf = "_start"},
        Lexer::Keyword::LD,
        Lexer::Keyword::V1,
        Lexer::Integer{.value = 0xEE},
        Lexer::Keyword::SUB,
        Lexer::Keyword::V1,
        Lexer::Integer{.value = 3},
        Lexer::Keyword::CALL,
        Lexer::Identifier{.idf = "_procedure"},
        Lexer::Keyword::JP,
        Lexer::Identifier{.idf = "_start"},
        Lexer::Plus{},
        Lexer::Integer{.value = 2},
    };
    

    EXPECT_EQ(expected, processed);
}