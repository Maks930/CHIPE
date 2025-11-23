file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(graphics SHARED ${headers} ${sources})



set_target_properties(graphics PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(graphics PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(graphics PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

target_include_directories(graphics
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(graphics
        PUBLIC
            glfw
            libglew_static
            glm::glm
        PRIVATE
            utils
)
