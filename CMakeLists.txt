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
)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY imgui.ini DESTINATION ${CMAKE_BINARY_DIR})

# Before we copy compile_commands.json to .vscode
# Making sure we should make sure if that dir already exists in the root of the projects directory
set(VSCODE_DIR "${CMAKE_CURRENT_LIST_DIR}/.vscode")
if(NOT EXISTS ${VSCODE_DIR})
    file(MAKE_DIRECTORY ${VSCODE_DIR})
endif()

# Copy to source directory
add_custom_target(
    copy-compile-commands ALL
    DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/compile_commands.json
)

# Creating a customize command specific for copying the compile_commands.json to users .vscode directory
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_LIST_DIR}/compile_commands.json
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_LIST_DIR}/.vscode/compile_commands.json
    DEPENDS
    # Unlike "proper" targets like executables and libraries, 
    # custom command / target pairs will not set up source
    # file dependencies, so we need to list file explicitly here
    generate-compile-commands
    ${CMAKE_BINARY_DIR}/compile_commands.json
)

# Generate the compilation commands. Necessary so cmake knows where it came
# from and if for some reason you delete it.
add_custom_target(generate-compile-commands
    DEPENDS
        ${CMAKE_BINARY_DIR}/compile_commands.json
)

target_link_libraries(
    ${PROJECT_NAME}
    Engine3D::Engine3D
)
