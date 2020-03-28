cmake_minimum_required(VERSION 3.5)

project(tglib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_USE_OPENSSL ON)

set (HEADERS
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

set (SOURCES ./src/sources/telegrambot.cpp
             ./src/sources/main.cpp)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED COMPONENTS Crypto SSL)
find_package(RapidJSON REQUIRED)

add_executable(tglib ${HEADERS} ${SOURCES})

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/third_party)
include_directories(${PROJECT_SOURCE_DIR}/third_party/magic_get/include/)

target_link_libraries(tglib stdc++fs Threads::Threads OpenSSL::SSL OpenSSL::Crypto)
