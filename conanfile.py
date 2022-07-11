from conan.tools.cmake import CMake, CMakeToolchain
from conan import ConanFile


class VideoCompareConan(ConanFile):
    name = "VideoCompare"
    version = "0.2.0"
    license = "Apache"
    author = "SU Tan sutan@insta360.com"
    url = "https://github.com/SuTanTank/VideoCompare"
    description = "Video Compare Tool"
    topics = ("Video", "Compare", "OpenCV")
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    build_policy = "missing"
    exports_sources = "*"

    def requirements(self):
        self.requires("opencv/[>=4.5.3]@insta360/prebuilt")

    def generates(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def imports(self):
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)

    def package(self):
        self.copy("*VideoCompare*", dst='bin', keep_path=False)

    def deploy(self):
        self.copy("*VideoCompare*", dst='VideoCompare', keep_path=False)

    def package_info(self):
        pass

    def package_id(self):
        self.info.settings.compiler = "ANY"
