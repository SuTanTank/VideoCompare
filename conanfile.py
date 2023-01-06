from conan.tools.cmake import CMake, CMakeToolchain
from conan import ConanFile
from os.path import join


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
        dst = ""
        if self.settings.compiler == "Visual Studio":   
            dst = join(dst, str(self.settings.build_type))
        self.copy("*.dll", dst=join("bin", dst), keep_path=False)
        self.copy("*.dylib", dst=join("lib", dst), keep_path=False)
        self.copy("*.so", dst=join("lib", dst), keep_path=False)

    def package(self):
        self.copy("*VideoCompare*", dst='bin', keep_path=False)

    def deploy(self):
        self.copy("*VideoCompare*", dst='VideoCompare', keep_path=False)

    def package_info(self):
        pass

    def package_id(self):
        self.info.settings.compiler = "ANY"
