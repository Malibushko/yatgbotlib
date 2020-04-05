# Yet another Telegram Bot Library for C++

Library for Telegram Bot Api written in C++ 

## Requirements

Compiler with C++17  
CMake v 3.5 or higher  
Conan 

### Build instructions (Conan)
```
conan remote add https://api.bintray.com/conan/yehorka9991/magic_get 
```
This will add repository with library recipe to your remotes
Then you can add library by adding 
```cpp
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(REQUIRES tglib/0.1@yehorka/stable
                BASIC_SETUP 
                BUILD missing)
add_executable (${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})
```

Now you can copy examples from `/examples` folder. 

## Running the tests

Tests are run automatically. If you dont want to run the tests set `"build_tests"` option to false. You can do this in your conanfile.txt 
```
[options]
tglib::build_tests = False
```
Or add modify code above
```cpp
conan_cmake_run(REQUIRES tglib/0.1@yehorka/stable
                BASIC_SETUP 
                BUILD missing
                OPTIONS tglib:build_tests=False)
```

## Examples

There are some examples of bots in `examples` folder.  
#### Echo bot
```cpp
#include <telegram_bot.h>

int main() {
    using namespace telegram;
    Bot bot{BOT_TOKEN};
    bot.onUpdate([&](Update&& update){
       if (update.message) {
           bot.reply(update.message.value(),update.message.value().text.value_or("<no text>"));
       }
    });
    bot.start();
}
```

#### Command bot 
```cpp
#include <telegram_bot.h>

int main() {
    using namespace telegram;
    Bot bot{BOT_TOKEN};
    // or you can you bot.onMessage()
    bot.onEvent<MessageCallback>("/start",[&](Message&& m){
       bot.sendMessage(m.chat.id,"This is bot that can reply to some commands.");
    });
    bot.onEvent<MessageCallback>("/help",[&](Message&& m){
        bot.sendMessage(m.chat.id,"List of allowed commands:\n\t/help\n\t/start\n\t/number\n");
    });
    bot.onEvent<MessageCallback>("/number",[&](Message&& m){
        bot.sendMessage(m.chat.id,std::to_string(rand() % 150));
    });
    // regexes have the lowest priority
    // so the bot will respons this callback if and only
    // if it will not match any callback or command
    bot.onEvent<MessageCallback>(std::regex{"(.*)"},[&](Message&& m){
       bot.sendMessage(m.chat.id,"Command is not supported. See list of supported commantd /h");
    });
    bot.setMyCommands({{"/start","get bot description"},
                        {"/help","get list of allowed commands"},
                        {"/number","get random number"}});
    // you can use regexes for any <callback>Callback signature;
    bot.start(100);
}

```
See more in `examples` folder. 

### Plans for future

1. Builtin ORM support 
2. More intuitive sequences 
3. Decreasing compilation speed 

## License

The project is under MIT license so it is free for usage. 



