from conans import ConanFile, CMake

class TGlibConan(ConanFile):
   name = "tglib"
   description = "Yet another telegram bot lib for C++"
   version = "0.1"
   license = "MIT"
   url = "https://github.com/conan-io/conan-center-index"
   settings = "os", "compiler", "build_type", "arch"
   generators = "cmake"
   requires = "rapidjson/1.1.0","cpp-httplib/0.5.7","gtest/1.10.0","magic_get/1.0"
   options = {"build_examples": [True, False], "build_tests": [True,False]}
   default_options = {"build_examples": False, "build_tests":True, "cpp-httplib:with_openssl": True}

   def source(self):
       self.run("git clone https://github.com/Malibushko/yatgbotlib.git")
       

   def build(self):
      cmake = CMake(self)
      cmake.verbose = True
      
      examples = "-DTGLIB_BUILD_EXAMPLES=ON" if self.options.build_examples else "-DTGLIB_BUILD_EXAMPLES=OFF"
      tests = "-DTGLIB_BUILD_TESTS=ON" if self.options.build_tests else "-DTGLIB_BUILD_TESTS=OFF";
      self.run("cmake yatgbotlib/ %s %s" % (examples,tests))
      cmake.build();
      if self.options.build_examples:
         self.run("ctest");

   def package(self):
      self.copy("*", dst="bin", src="./bin") # From bin to bin
      self.copy("*", dst="lib", src="./lib") # From lib to bin
      self.copy("*.h", dst="include", src = "./yatgbotlib/include")
      self.copy("*", dst="include/utility", src="./yatgbotlib/src/utility")
      self.copy("*", dst = "include/headers", src = "./yatgbotlib/src/headers")

   def package_info(self):
        self.cpp_info.libs = ["tglib"]
