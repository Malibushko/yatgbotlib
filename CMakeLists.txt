cmake_minimum_required(VERSION 3.5)

project(curl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_USE_OPENSSL ON)

set (HEADERS
             ./telegram_structs.h
             ./telegram_bot.h
             ./json_parser.h
             ./traits.h
             ./querybuilder.h
             ./apimanager.h
             ./trie.h
             ./update_manager.h
             ./sequence_dispatcher.h
             ./networkmanager.h
             ./include/httplib.h)

set (SOURCES ./telegrambot.cpp
             ./main.cpp)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED COMPONENTS Crypto SSL)
add_executable(curl ${HEADERS} ${SOURCES})

include_directories(${PROJECT_SOURCE_DIR}/include)
target_link_libraries(curl stdc++fs Threads::Threads OpenSSL::SSL OpenSSL::Crypto)
