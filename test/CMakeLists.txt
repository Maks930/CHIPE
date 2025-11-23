FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

add_library(basic_tests INTERFACE)
target_link_libraries(basic_tests INTERFACE
        gtest gtest_main gmock_main fmt::fmt
)

add_executable(PreprocessorTest preproctest.cpp)
target_link_libraries(PreprocessorTest basic_tests cas_lib)
