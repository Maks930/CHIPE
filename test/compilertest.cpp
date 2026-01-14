#include <gtest/gtest.h>
#include <fmt/base.h>

#include <Lexer.h>
#include <Compiler.h>
#include <variant>


TEST(Preprocessor, Case_Define) {
    std::vector<Lexer::Token> in_data{
        Lexer::Keyword::JP,
        Lexer::Identifier{.idf = "_start"},
        Lexer::Mark{.idf = "_procedure"},
        Lexer::Keyword::SUB,
        Lexer::Register::V1,
        Lexer::Register::V2,
        Lexer::Keyword::RET,
        Lexer::Mark{.idf = "_start"},
        Lexer::Keyword::LD,
        Lexer::Register::V1,
        Lexer::Integer{.value = 0xFF},
        Lexer::Keyword::LD,
        Lexer::Register::V2,
        Lexer::Integer{.value = 1},
        Lexer::Keyword::CALL,
        Lexer::Identifier{.idf = "_procedure"},
        Lexer::Keyword::HLT,
        Lexer::Keyword::JP,
        Lexer::Identifier{.idf = "_start"},
        Lexer::Plus{},
        Lexer::Integer{.value = 4}
    };

    Compiler comp;
    comp.first_cycle(in_data);

    fmt::print("Makrs:\n");
    for (const auto& p : comp.getMarks()) {
        fmt::print("{}: {:#04X}\n", p.first, p.second);
    }fmt::print("\n\n");

    auto processed = comp.compile(in_data);
    std::vector<u8> expected{
        0x12, 0x06, 0x81, 0x25, 0x00, 0xEE, 0x61, 0xFF, 0x62, 0x01, 0x22, 0x02, 0x00, 0x00, 0x12, 0x0A
    };

    EXPECT_EQ(expected, processed);
}