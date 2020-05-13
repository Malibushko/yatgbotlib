cmake_minimum_required(VERSION 3.5)

project(tglib VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_USE_OPENSSL ON)
set(CMAKE_BUILD_TYPE RELEASE)


set(TGLIB_SOURCE_DIR ${CMAKE_SOURCE_DIR})
set(TGLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

set(TGLIB_BUILD_TESTS ON)
set(TGLIB_BUILD_EXAMPLES ON)

set(HEADERS
    ./include/telegram_bot.h
    ./src/headers/json_parser.h
    ./src/headers/querybuilder.h
    ./src/headers/apimanager.h
    ./src/headers/update_manager.h
    ./src/headers/sequence_dispatcher.h
    ./src/headers/networkmanager.h
    ./src/headers/telegram_structs.h
    ./src/utility/logger.h
    ./src/utility/traits.h
    ./src/utility/trie.h
    ./src/utility/utility.h
    ./src/utility/threadpool.h)

set (SOURCES ./src/sources/telegrambot.cpp
    ./src/sources/update_manager.cpp
    ./src/sources/networkmanager.cpp
    ./src/sources/querybuilder.cpp)

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

add_library(tglib SHARED ${HEADERS} ${SOURCES})


include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PUBLIC stdc++fs CONAN_PKG::rapidjson
    CONAN_PKG::cpp-httplib CONAN_PKG::magic_get)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER include/telegram_bot.h)

if (TGLIB_BUILD_TESTS)
    enable_testing()
    add_subdirectory(${PROJECT_SOURCE_DIR}/test)
endif()

if (TGLIB_BUILD_EXAMPLES)
    add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
endif()

install(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/${PROJECT_NAME}
    )
