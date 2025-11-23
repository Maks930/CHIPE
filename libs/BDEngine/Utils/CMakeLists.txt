file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE inline ${CMAKE_CURRENT_SOURCE_DIR}/*.inl)

add_library(utils SHARED ${headers} ${sources} ${inline})

set_target_properties(utils PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(utils PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(utils PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

target_include_directories(utils
        PUBLIC
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)

