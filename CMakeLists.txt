cmake_minimum_required(VERSION 3.5)

project(tglib VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_USE_OPENSSL ON)

set(TGLIB_SOURCE_DIR ${CMAKE_SOURCE_DIR})
set(TGLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(TGLIB_BUILD_TESTS ON)
set(HEADERS
             ./src/headers/json_parser.h
             ./src/headers/querybuilder.h
             ./src/headers/apimanager.h
             ./src/headers/update_manager.h
             ./src/headers/sequence_dispatcher.h
             ./src/headers/networkmanager.h
             ./include/telegram_bot.h
             ./include/telegram_structs.h
             ./src/utility/logger.h
             ./src/utility/traits.h
             ./src/utility/trie.h
             ./src/utility/utility.h)

set (SOURCES ./src/sources/telegrambot.cpp)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED COMPONENTS Crypto SSL)
find_package(RapidJSON REQUIRED)

add_library(tglib SHARED ${HEADERS} ${SOURCES})

set_target_properties(tglib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(tglib PROPERTIES PUBLIC_HEADER include/telegram_bot.h)

include_directories(${PROJECT_SOURCE_DIR}/src
                    ${PROJECT_SOURCE_DIR}/include
                    ${PROJECT_SOURCE_DIR}/third_party
                    ${PROJECT_SOURCE_DIR}/third_party/magic_get/include)

target_link_libraries(tglib stdc++fs Threads::Threads OpenSSL::SSL OpenSSL::Crypto)

if (TGLIB_BUILD_TESTS)
    enable_testing()
    add_subdirectory(${PROJECT_SOURCE_DIR}/test)
endif()
add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
