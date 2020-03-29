
find_package(GTest REQUIRED)
find_package(OpenSSL REQUIRED)
set(TEST_IMPORTED_LIBRARIES GTest::GTest OpenSSL::SSL OpenSSL::Crypto)
if (NOT ${GTEST_FOUND})
    message("GTest library not found. ")
endif()

macro (m_add_test _TEST_NAME)
    add_executable(${_TEST_NAME}_tests ${_TEST_NAME}_tests.cpp)
    target_link_libraries(${_TEST_NAME}_tests ${TEST_IMPORTED_LIBRARIES})
    add_test(NAME ${_TEST_NAME} COMMAND ${_TEST_NAME}_tests)
endmacro()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${GTEST_INCLUDE_DIRS}
                    ${TGLIB_SOURCE_DIR})

m_add_test(api_manager)
m_add_test(json_parser)
m_add_test(query_builder)
m_add_test(sequence_dispatcher)
m_add_test(network_manager)
m_add_test(bot)
