cmake_minimum_required (VERSION 3.27)
project(Editor)
set(CMAKE_CXX_STANDARD 20)

build_application(
    SOURCES
    Application.cpp
    EditorLayer.cpp
    Panels/SceneHeirarchyPanel.cpp
    Panels/ContentBrowserPanel.cpp

    PACKAGES
    spdlog
    glfw3
    OpenGL
    imguidocking
    stb
    box2d
    glad
    glm
    EnTT
    imguizmo
    yaml-cpp
    nfd
    engine-3d


    LINK_PACKAGES
    glfw
    ${OPENGL_gl_LIBRARY}
    spdlog::spdlog
    box2d::box2d
    imguidocking::imguidocking
    stb::stb
    glad::glad
    glm::glm
    EnTT::EnTT
    yaml-cpp::yaml-cpp
    imguizmo::imguizmo
    nfd::nfd
    Engine3D::Engine3D
)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY imgui.ini DESTINATION ${CMAKE_BINARY_DIR})

generate_compile_commands()