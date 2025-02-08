cmake_minimum_required(VERSION 3.26)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(
    Ruya
    VERSION 0.0.0
)


####################################################################################################
#####     ruya
####################################################################################################

set(HEADERS
    app.h
    engine/core/window.h
    engine/render/renderer.h
    engine/render/shader.h
    engine/scene/camera.h
    engine/scene/light_source.h
    engine/scene/material.h
    engine/scene/mesh.h
    engine/scene/object.h
    engine/scene/scene.h
    engine/scene/texture.h
    engine/scene/models/cube.h
    engine/scene/models/icosahedron.h
    engine/scene/models/square.h
    utils/uuid.h
    utils/timer.h
    io/stb_image.h
)
    
set(SOURCES
    # main.cpp
    # test_app.hpp
    engine/core/window.cpp
    engine/render/renderer.cpp
    engine/render/shader.cpp
    engine/scene/camera.cpp
    engine/scene/light_source.cpp
    engine/scene/material.cpp
    engine/scene/mesh.cpp
    engine/scene/object.cpp
    engine/scene/scene.cpp
    engine/scene/texture.cpp
    engine/scene/models/cube.cpp
    engine/scene/models/icosahedron.cpp
    engine/scene/models/icosphere.hpp
    engine/scene/models/square.cpp
    utils/uuid.cpp
    utils/timer.cpp
    io/stb_image.cpp
)

# Compiler definitions
set(DEFINES

)

# Compiler options
set(OPTIONS

)


# The source and header files defined here are assumed to be within the src/ folder
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
list(TRANSFORM HEADERS PREPEND "src/")
list(TRANSFORM SOURCES PREPEND "src/")


# targets to be used throughout our build process
set(MAIN_TARGET "main")


# create main target and add its sources
add_executable(${MAIN_TARGET})
target_sources(${MAIN_TARGET} PRIVATE 
    ${SOURCES} 
    ${HEADERS}
    "src/main.cpp"
    "src/test_app.hpp"
)


# add the src dir as include so that it can serve as the root for all our includes
#   =>  no matter where a source file is located within src/, it can include any other 
#       src file by providing the path starting from src/ as root, 
#           i.e. `#include "mydir/myfile.h"` includes the file located at src/mydir/myfile.h
target_include_directories(${MAIN_TARGET} PRIVATE ${SRC_DIR})


####################################################################################################
#####     DEPENDENCIES
####################################################################################################
set(DEPENDENCY_LIBS)
set(DEPENDENCY_PRIVATE_INCLUDES)

set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/external")

# include the "external" dir
target_include_directories(${MAIN_TARGET} PRIVATE "${EXTERNAL_DIR}")
list(APPEND DEPENDENCY_PRIVATE_INCLUDES "${EXTERNAL_DIR}")


##### GLM
set(LINK_GLM_STATIC 0) # set to 1 if you want to link glm statically
set(GLM_DIR "${EXTERNAL_DIR}/glm")
if (LINK_GLM_STATIC)
    message(LINK_GLM_STATIC=${LINK_GLM_STATIC})
    set(BUILD_STATIC_LIBS 1)
    add_subdirectory(${GLM_DIR})
    list(APPEND DEPENDENCY_LIBS glm_static)
endif (LINK_GLM_STATIC)

##### GLFW3
set(GLFW_DIR "${EXTERNAL_DIR}/glfw")
add_subdirectory(${GLFW_DIR})
list(APPEND DEPENDENCY_LIBS glfw)
# set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)



##### GLAD
set(GLAD_DIR "${EXTERNAL_DIR}/glad")

add_library(LibGlad "${GLAD_DIR}/src/glad.c")
target_include_directories(LibGlad PRIVATE "${GLAD_DIR}/include")

list(APPEND DEPENDENCY_LIBS LibGlad)
list(APPEND DEPENDENCY_PRIVATE_INCLUDES "${GLAD_DIR}/include")


##### WHEREAMI: to get the location of the executable path
# Repository: https://github.com/gpakosz/whereami/tree/master
# Commit ID: e1087ed050b1ecf4f26e952362197038ca88168c
#   -> C++ implementation is added recently and is in a different branch
set(WAI_DIR "${EXTERNAL_DIR}/whereami")
add_library(LibWhereAmI
    "${WAI_DIR}/src/whereami.c"
    "${WAI_DIR}/src/whereami++.cpp"
)
target_include_directories(LibWhereAmI PRIVATE "${WAI_DIR}/include/whereami")

list(APPEND DEPENDENCY_LIBS LibWhereAmI)
list(APPEND DEPENDENCY_PRIVATE_INCLUDES "${WAI_DIR}/include")

##### IMGUI
set(IMGUI_DIR "${EXTERNAL_DIR}/imgui")

set(IMGUI_SOURCES
    # "${IMGUI_DIR}/imconfig.h"
    "${IMGUI_DIR}/imgui.cpp"
    # "${IMGUI_DIR}/imgui.h"
    # "${IMGUI_DIR}/imstb_rectpack.h"
    # "${IMGUI_DIR}/imstb_textedit.h"
    # "${IMGUI_DIR}/imstb_truetype.h"
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    # "${IMGUI_DIR}/imgui_internal.h"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
    # "${IMGUI_DIR}/backends/imgui_impl_opengl3.h"
    # "${IMGUI_DIR}/backends/imgui_impl_opengl3_loader.h"
    # "${IMGUI_DIR}/backends/imgui_impl_glfw.h"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp"
    # "${IMGUI_DIR}/misc/cpp/imgui_stdlib.h"
)

add_library(LibImGui STATIC ${IMGUI_SOURCES})
target_include_directories(LibImGui PRIVATE 
    "${IMGUI_DIR}" 
    "${IMGUI_DIR}/backends"
)


list(APPEND DEPENDENCY_PRIVATE_INCLUDES 
    "${IMGUI_DIR}" 
    "${IMGUI_DIR}/backends"
    "${IMGUI_DIR}/misc/cpp"
)

target_link_libraries(LibImGui glfw)

list(APPEND DEPENDENCY_LIBS LibImGui)


##### link & include libraries to executable targets
message(">>>>>>>>>>>>> DEPENDENCY_LIBS: ${DEPENDENCY_LIBS}")
message(">>>>>>>>>>>>> DEPENDENCY_PRIVATE_INCLUDES: ${DEPENDENCY_PRIVATE_INCLUDES}")

target_link_libraries(${MAIN_TARGET} ${DEPENDENCY_LIBS})
target_include_directories(${MAIN_TARGET} PRIVATE ${DEPENDENCY_PRIVATE_INCLUDES})


####################################################################################################
#####     RESOURCES
####################################################################################################

##### Resource files: copy resource files (images, txt, shaders) to the executable location 
# Copy the src/engine/render/shaders folder to shaders/ relative to the destination folder
# of the main target.
add_custom_command(
    TARGET ${MAIN_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SRC_DIR}/engine/render/shaders
    $<TARGET_FILE_DIR:${MAIN_TARGET}>/shaders
)

add_custom_command(
    TARGET ${MAIN_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    $<TARGET_FILE_DIR:${MAIN_TARGET}>/resources
)