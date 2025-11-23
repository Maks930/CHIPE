add_subdirectory(Utils)
add_subdirectory(Core)
add_subdirectory(System)
add_subdirectory(Graphics)

add_library(BadEngine SHARED include/BadEngine/Engine.h)
target_link_libraries(BadEngine PUBLIC utils core system graphics)

target_include_directories(BadEngine
        PUBLIC
            include/
)