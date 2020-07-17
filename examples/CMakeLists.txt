cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TG_BOT_TOKEN "")

list (APPEND TARGET_IMPORTED_LIBS ${PROJECT_NAME})

macro (add_example _EXAMPLE_NAME)
    add_executable(${_EXAMPLE_NAME} ${_EXAMPLE_NAME}.cpp)
    target_link_libraries(${_EXAMPLE_NAME} PRIVATE ${TARGET_IMPORTED_LIBS})
endmacro()

add_definitions(-DBOT_TOKEN=\"${TG_BOT_TOKEN}\"
    -DRESOURCE_PATH=\"${TGLIB_SOURCE_DIR}/examples/resource/\")

add_example(echo_bot)
add_example(webhook_example)
add_example(callback_example)
add_example(register_bot)
add_example(sequence_example)
add_example(regex_example)
