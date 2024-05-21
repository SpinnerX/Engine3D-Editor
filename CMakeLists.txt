cmake_minimum_required (VERSION 3.15)

set(PROJECT_NAME Editor)
set(CMAKE_CXX_STANDARD 20)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "") # works (in creating the compile_commands.json file)
project(${PROJECT_NAME})

find_package(Engine3D REQUIRED)

# find_package(glfw3 REQUIRED)
find_package(glfw3 REQUIRED)

# find_package(OpenGL REQUIRED)
# find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
if(APPLE)
find_package(GLEW REQUIRED)
endif(APPLE)
# find_package(imgui REQUIRED)
# find_package(ImGuizmo REQUIRED)
find_package(imgui REQUIRED)
find_package(ImGuizmo REQUIRED)

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(glad REQUIRED)
find_package(glm REQUIRED)

find_package(box2d REQUIRED)
find_package(yaml-cpp REQUIRED)

add_executable(
	${PROJECT_NAME}
	Application.cpp
    EditorLayer.cpp
    Panels/SceneHeirarchyPanel.cpp
    Panels/ContentBrowserPanel.cpp
    UI/UI.h
)

file(COPY ${CMAKE_SOURCE_DIR}/imgui.ini DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/Resources DESTINATION ${CMAKE_BINARY_DIR})

target_link_libraries(
    ${PROJECT_NAME}
    Engine3D::Engine3D
)
