cmake_minimum_required(VERSION 4.1)
project(CHIPE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY  https://github.com/fmtlib/fmt
        GIT_TAG         12.0.0
        GIT_PROGRESS    TRUE
)

FetchContent_Declare(
        openal
        GIT_REPOSITORY  https://github.com/kcat/openal-soft
        GIT_TAG         1.24.3
        GIT_PROGRESS    TRUE
)


set(CMAKE_CXX_STANDARD 20)

FetchContent_MakeAvailable(fmt openal)

set(GLM_ENABLE_CXX_20 ON)

set(ONLY_LIBS  ON CACHE BOOL "" FORCE)
set(GLEW_STATIC ON)
set(GLFW_LIBRARY_TYPE SHARED CACHE INTERNAL STRING)

add_subdirectory(libs)
add_subdirectory(app)
add_subdirectory(tools)



