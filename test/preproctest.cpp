#include <gtest/gtest.h>
#include <fmt/base.h>

#include <Preprocessor.h>

TEST(Preprocessor, Case_Define) {

    std::string in_data = "; Test Preprocessor\n%define M_PI 3.14 ; Define PI number\nLD V[0], M_PI";
    std::string out_data = "LD V[0], 3.14";
    std::string res_data = Preprocessor::preprocess(in_data);

    fmt::print("\nInput Data\n{}\n", in_data);
    fmt::print("\nOutput Data\n{}\n", res_data);
    fmt::print("\nNeed Data\n{}\n", out_data);


    EXPECT_EQ(res_data, out_data);
}