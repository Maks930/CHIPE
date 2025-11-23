add_subdirectory(glew)
add_subdirectory(glfw)
add_subdirectory(glm)

set(imgui_sources
        ./imgui/imgui.cpp
        ./imgui/imgui_demo.cpp
        ./imgui/imgui_draw.cpp
        ./imgui/imgui_tables.cpp
        ./imgui/imgui_widgets.cpp
        ./imgui/backends/imgui_impl_glfw.cpp
        ./imgui/backends/imgui_impl_opengl3.cpp
)

set(imgui_headers
        ./imgui/imgui.h
        ./imgui/backends/imgui_impl_glfw.h
        ./imgui/backends/imgui_impl_opengl3.h
        ./imgui/imconfig.h
)

add_library(imgui SHARED ${imgui_headers} ${imgui_sources})
target_link_libraries(imgui PRIVATE glfw)
target_include_directories(imgui
        PUBLIC
        ./imgui
)

add_subdirectory(BDEngine)

set(GLM_BUILD_TESTS OFF)