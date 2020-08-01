from conans import ConanFile, CMake
import sys
import os

class TGlibConan(ConanFile):
   name = "tglib"
   description = "Yet another telegram bot lib for C++"
   version = "0.1"
   license = "MIT"
   url = "https://github.com/conan-io/conan-center-index"
   settings = "os", "compiler", "build_type", "arch"
   generators = "cmake"
   requires = "rapidjson/1.1.0","cpp-httplib/0.5.7","gtest/1.10.0","magic_get/1.0"
   options = {"build_examples": [True, False], "build_tests": [True,False],"verbosity_level":[0,1,2]}
   default_options = {"build_examples": False, "build_tests":True, "cpp-httplib:with_openssl": True,"verbosity_level":1}

   def source(self):
      self.run("git clone https://github.com/Malibushko/yatgbotlib.git")
      try:
          if os.environ['TRAVIS_BUILD']:
              print("Building on Travis")
              self.run("pip install html2markdown lxml beautifulsoup4")
      except KeyError: 
          print("Building on local machine")
          self.run("pip install --user html2markdown lxml beautifulsoup4") 
          
           
   def configure_cmake(self):
       cmake = CMake(self)

       cmake.definitions["TGLIB_BUILD_EXAMPLES"] = "ON" if self.options.build_examples else "OFF"
       cmake.definitions["TGLIB_BUILD_TESTS"] = "ON" if self.options.build_tests else "OFF"
       cmake.definitions["VERBOSITY_LEVEL"] = self.options.verbosity_level
       print("Building tests: {}\nBuilding examples: {}\nLogger verbosity level: {}"
       	.format(cmake.definitions["TGLIB_BUILD_TESTS"],cmake.definitions["TGLIB_BUILD_EXAMPLES"],cmake.definitions["VERBOSITY_LEVEL"]))
       cmake.configure()
       return cmake

   def build(self):
      cmake = self.configure_cmake()
      cmake.verbose = True
      cmake.build()
      if self.options.build_tests:
         cmake.test()

   def package(self):
      self.copy("*", dst="bin", src="./bin") # From bin to bin
      self.copy("*", dst="lib", src="./lib") # From lib to bin
      self.copy("*.h", dst="include", src = "./yatgbotlib/include")
      
      self.copy("*", dst="include/utility", src="./yatgbotlib/src/utility")
      self.copy("*", dst = "include/headers", src = "./yatgbotlib/src/headers")

   def package_info(self):
        self.cpp_info.libs = ["tglib"]
