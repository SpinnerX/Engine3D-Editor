cmake_minimum_required (VERSION 3.15)

set(PROJECT_NAME Editor)
set(CMAKE_CXX_STANDARD 20)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "") # works (in creating the compile_commands.json file)
project(${PROJECT_NAME})

find_package(Engine3D REQUIRED)

add_executable(
	${PROJECT_NAME}
	Application.cpp
    EditorLayer.cpp
    Panels/SceneHeirarchyPanel.cpp
    Panels/ContentBrowserPanel.cpp
)

target_link_libraries(
    ${PROJECT_NAME}
    Engine3D::Engine3D
)
