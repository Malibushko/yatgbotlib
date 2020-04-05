if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
        "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

set(TEST_IMPORTED_LIBRARIES tglib stdc++fs CONAN_PKG::gtest)

set(BOT_TOKEN "663528116:AAG-0bX4VLsnkMVzDpA4Ak_8dWMgRquD3ig")
set(TEST_CHAT 250033715)

macro (m_add_test _TEST_NAME)
    add_executable(${_TEST_NAME}_tests ${_TEST_NAME}_tests.cpp)
    target_link_libraries(${_TEST_NAME}_tests ${TEST_IMPORTED_LIBRARIES})
    add_test(NAME ${_TEST_NAME} COMMAND ${_TEST_NAME}_tests)
endmacro()
add_definitions(-DTEST_PATH=\"${TGLIB_SOURCE_DIR}/test/resource/\"
                -DBOT_TOKEN=\"${BOT_TOKEN}\"
                -DTEST_CHAT_ID=${TEST_CHAT})


m_add_test(api_manager)
m_add_test(json_parser)
m_add_test(query_builder)
m_add_test(sequence_dispatcher)
m_add_test(bot)
