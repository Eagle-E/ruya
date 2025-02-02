set(SOURCES
    ./imconfig.h
    ./imgui.cpp
	./imgui.h
	./imstb_rectpack.h
	./imstb_textedit.h
	./imstb_truetype.h
    ./imgui_demo.cpp
    ./imgui_draw.cpp
    ./imgui_internal.h
    ./imgui_tables.cpp
    ./imgui_widgets.cpp
    ./backends/imgui_impl_opengl3.cpp
    ./backends/imgui_impl_opengl3.h
    ./backends/imgui_impl_glfw.h
    ./backends/imgui_impl_glfw.cpp
)

add_library(LibImGui STATIC ${SOURCES})
target_include_directories(LibImGui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_CURRENT_SOURCE_DIR}/backends)