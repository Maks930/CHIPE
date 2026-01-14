
add_subdirectory(GfSys)

file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*/*.h)
file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*/*.cpp)

if (${CMAKE_SYSTEM} MATCHES "Linux")
    set(PRIVATE_LINKED_LIBS X11)
elseif (${CMAKE_SYSTEM} MATCHES "Win")
    set(PRIVATE_LINKED_LIBS -static -lstdc++exp -static-libgcc -static-libstdc++ winpthread -lgdi32 -lopengl32)
endif()

add_custom_target(copy_data
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/../res/"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
        COMMENT "Copying data"
)



add_executable(CHIPE main.cpp ${sources} Application/GuiS/Guis.cpp Application/AudioEngine/AEngine.cpp)
target_link_libraries(CHIPE system fmt::fmt imgui libglew_shared glfw OpenAL::OpenAL ${PRIVATE_LINKED_LIBS})
add_dependencies(CHIPE copy_data)
include_directories(.)

target_compile_definitions(CHIPE PUBLIC
        APPLICATION_NAME="CHIP-8"
        TIPS_MIN=1
        TIPS_MAX=10000
        TIPS_STANDARD=750
)