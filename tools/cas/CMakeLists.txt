file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*/*.h)
file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*/*.cpp)


add_library(cas_lib STATIC ${headers} ${sources})
target_include_directories(cas_lib PUBLIC ./Preprocessor ./Lexer ./Utils)
target_link_libraries(cas_lib fmt::fmt)

add_executable(cas main.cpp)
target_link_libraries(cas ${PRIVATE_LINKED_LIBS} cas_lib)
include_directories(.)

#target_compile_definitions(cas PUBLIC
#
#)