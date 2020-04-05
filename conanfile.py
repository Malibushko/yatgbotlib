from conans import ConanFile, CMake

class TGlibConan(ConanFile):
   name = "tglib"
   description = "Yet another telegram bot lib for C++"
   version = "0.1"
   settings = "os", "compiler", "build_type", "arch"
   generators = "cmake"

   def source(self):
      self.run("git clone https://github.com/Malibushko/yatgbotlib.git")

   def imports(self):
      self.copy("*", dst="bin", src="bin") # From bin to bin
      self.copy("*", dst="lib", src="lib") # From lib to bin
      self.copy("*.h", dst="include", src = "include")
      self.copy("*", dst="include/utility", src="src/utility")
      self.copy("*", dst = "include/headers", src = "src/headers")

   def build(self):
      cmake = CMake(self)
      cmake.verbose = True
      cmake.configure()
      cmake.build()