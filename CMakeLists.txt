cmake_minimum_required(VERSION 3.5)

project(tglib VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_USE_OPENSSL ON)

set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(TGLIB_SOURCE_DIR ${CMAKE_SOURCE_DIR})
set(TGLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

set(TGLIB_BUILD_TESTS ON)
set(TGLIB_BUILD_EXAMPLES OFF)
set(VERBOSITY_LEVEL 1)

find_package(PythonInterp 3 REQUIRED)
if (!${PYTHONINTERP_FOUND})
    message("Python interperator was not found. Aborting")
elseif()
    message("Python ${PYTHON_VERSION_STRING} was found")
endif()

set(HEADERS_PATH ${CMAKE_SOURCE_DIR}/src/headers)
set(SOURCES_PATH ${CMAKE_SOURCE_DIR}/src/sources)
set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include)
set(UTILITY_PATH ${CMAKE_SOURCE_DIR}/src/utility)

# Generation of API
execute_process(
 COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/generate.py
    # generate structs
    "${HEADERS_PATH}/__telegram_structs.h" "${HEADERS_PATH}/telegram_structs.h"
    "${INCLUDE_PATH}/__telegram_bot.h" "${INCLUDE_PATH}/telegram_bot.h"
    "${SOURCES_PATH}/__telegram_bot.cpp" "${SOURCES_PATH}/telegram_bot.cpp"
)
set(HEADERS
    ${INCLUDE_PATH}/telegram_bot.h
    ${HEADERS_PATH}/json_parser.h
    ${HEADERS_PATH}/querybuilder.h
    ${HEADERS_PATH}/apimanager.h
    ${HEADERS_PATH}/update_manager.h
    ${HEADERS_PATH}/sequence_dispatcher.h
    ${HEADERS_PATH}/networkmanager.h
    ${HEADERS_PATH}/telegram_structs.h
    ${UTILITY_PATH}/logger.h
    ${UTILITY_PATH}/traits.h
    ${UTILITY_PATH}/trie.h
    ${UTILITY_PATH}/utility.h
    ${UTILITY_PATH}/threadpool.h)

set (SOURCES ${SOURCES_PATH}/telegram_bot.cpp
    ${SOURCES_PATH}/update_manager.cpp
    ${SOURCES_PATH}/networkmanager.cpp
    ${SOURCES_PATH}/querybuilder.cpp)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
        "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(CONANFILE conanfile.txt
    BASIC_SETUP
    CMAKE_TARGETS
    BUILD missing)

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})

message("Verbosity level: ${VERBOSITY_LEVEL}")
add_definitions(-DTGLIB_VERBOSITY_LEVEL=${VERBOSITY_LEVEL})

include_directories(${SOURCES_PATH} ${HEADERS_PATH} ${INCLUDE_PATH})

list (APPEND TARGET_LIBRARIES
    CONAN_PKG::rapidjson
    CONAN_PKG::cpp-httplib
    CONAN_PKG::magic_get
    CONAN_PKG::spdlog
    CONAN_PKG::fmt)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    list(APPEND TARGET_LIBRARIES c++experimental)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    list(APPEND TARGET_LIBRARIES stdc++fs)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC ${TARGET_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION 1.0)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER include/telegram_bot.h)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using Clang
    add_compile_options(-Wall -pedantic-errors)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
    add_compile_options(-Wall -pedantic-errors)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++
    add_compile_options(/Zc:__cplusplus /permissive- /Wall)
endif()

if (TGLIB_BUILD_TESTS)
    message("Building tests")
    enable_testing()
    add_subdirectory(${PROJECT_SOURCE_DIR}/test)
endif()

if (TGLIB_BUILD_EXAMPLES)
    message("Building examples")
    add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
endif()
