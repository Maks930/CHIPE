#include <gtest/gtest.h>
#include <fmt/base.h>

#include <Preprocessor.h>

TEST(Preprocessor, Case_Define) {

    std::string in_data = "; Test Preprocessor\ndb 0xFF ; Define 0xFF byte\ndw 0xFFAC";
    std::string out_data = "db 0xFF\ndw 0xFFAC";
    std::string res_data = Preprocessor::preprocess(in_data);

    fmt::print("\nInput Data\n{}\n", in_data);
    fmt::print("\nOutput Data\n{}\n", res_data);
    fmt::print("\nNeed Data\n{}\n", out_data);


    EXPECT_EQ(res_data, out_data);
}