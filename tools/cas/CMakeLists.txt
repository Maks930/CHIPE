file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*/*.h)
file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*/*.cpp)

add_executable(cas main.cpp)
target_link_libraries(cas ${PRIVATE_LINKED_LIBS})
include_directories(.)

#target_compile_definitions(cas PUBLIC
#
#)