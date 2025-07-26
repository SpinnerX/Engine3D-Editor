from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout

class EditorRecipe(ConanFile):
    name = "engine3d-editor"
    version = "1.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    export_source = "CMakeLists.txt", "Application.cpp"

    # Putting all of your build-related dependencies here
    def build_requirements(self):
        self.tool_requires("make/4.4.1")
        self.tool_requires("cmake/3.27.1")
        self.tool_requires("engine3d-cmake-utils/4.0")

    # Putting all of your packages here
    def requirements(self):
        self.requires("box2d/2.4.2")
        self.requires("glad/0.1.36")
        self.requires("imguidocking/2.0")
        self.requires("imguizmo/1.0")
        self.requires("entt/3.15.0")
        self.requires("glfw/3.4", transitive_headers=True)
        self.requires("opengl/system", transitive_headers=True)
        self.requires("spdlog/1.15.1")
        self.requires("glm/1.0.1", transitive_headers=True)

        # Vulkan-related headers and includes packages
        self.requires("tinyobjloader/2.0.0-rc10")
        self.requires("tinygltf/2.9.0")
        self.requires("stb/cci.20230920")
        self.requires("yaml-cpp/0.8.0")
        self.requires("engine-3d/1.0")
        self.requires("nfd/1.0")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
    
    def layout(self):
        cmake_layout(self)