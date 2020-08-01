# Yet another Telegram Bot Library for C++

Library for Telegram Bot Api written in C++ 

## CI

Travis CI [![Build Status](https://travis-ci.org/Malibushko/yatgbotlib.svg?branch=master)](https://travis-ci.org/Malibushko/yatgbotlib)  
Appveyor  [![Build Status](https://img.shields.io/appveyor/ci/Malibushko/yatgbotlib/master.svg)](https://ci.appveyor.com/project/Malibushko/yatgbotlib)

## Requirements

Compiler with C++17 support  
CMake v 3.5 or higher  
Conan  
Python 3.x.x  


## Supported API version

API for this library generated automatically (if use Conan) during the build  
If you want to update API just rebuild the library as usual  
If you choose building by hands, to generate API you need the following python libraries:  
1) `lxml`  
2) `html2markdown`  
3) `beautifulsoup4`  
4) `requests`  

##### Note 

Python 3 and pip3 is required for automatic API generation, so make sure you have added it to your PATH
(e.g it can be run from terminal without specifying path explicitly) 

## Build instructions (Conan)
```
conan remote add <some_name> https://api.bintray.com/conan/yehorka9991/magic_get 
```
This will add repository with library recipe to your remotes or download `conanfile.py` from this repo and run   

`cd /path/to/conanfile`
`conan create .`
`conan run .`

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
##### Or 
You can add the following to your existing `conanfile.txt`  
```
[requires]
tglib/0.1@yehorka/stable
```
See more detailed information here https://github.com/conan-io/cmake-conan

Now you can copy and run examples from `/examples` folder. 

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
## Logging

By default spdlog is used. You can replace it by user-defined logger by calling  
`utility::Logger::set_implementation(your_logger_obj)`  
in C++ code.   
Your logger implementation should inherit `LoggerBase` class and implement all virtual functions.  
Logger has three levels of verbosity:
- 0 - no logging enabled
- 1 - only warnings or errors
- 2 - warnings, errors and additional information about executing functions and received results

By default 1 level is used. You can change it by specifying `verbosity_level` variable in conan build 
or `VERBOSITY_LEVEL` in CMakeLists.txt if you choosed manual building. 

Example of how your conanfile.txt may look like
```
[requires]
tglib/0.1

[generators]
cmake

[options]
tglib:verbosity_level=2
tglib:build_tests=False
```

## Examples

There are some examples of bots in `examples` folder.  
#### Registration bot (fills the form) 
```cpp
#include <telegram_bot.h>
struct User {
    std::string username;
    std::string password;
    std::string date_of_birth;
    std::string to_string() {
        std::string buffer;
        buffer.reserve(username.size()+password.size()+date_of_birth.size());
        buffer += "\nName: " + username +
                  "\nPassword: " + password +
                  "\nBirth date: " + date_of_birth + '\n';
        return buffer;
    }
};

int main() {
    using namespace telegram;
    Bot bot{BOT_TOKEN};
    bot.onMessage("/register",[&](const Message& msg){
       bot.sendMessage(msg.chat.id,"Send your username",ParseMode::Markdown);
       auto registration = std::make_shared<Sequence<MessageCallback>>();
       auto user = std::make_shared<::User>();
       // transition is an action that bot will do after each step

       // check is a function that is called BEFORE next step
       // and must return boolean value, and indicates
       // if value should be passed to next step or not

       registration->addTransition([user,&bot](const Message& m){
           user->username = m.text.value();
           bot.reply(m,"Send your password");
       })->addCheck([&](const Message& m){
           if (!m.text || m.text->size() < 3) {
               bot.reply(m,"Username is empty or too short");
               return false;
           }
           return true; // must always return boolean value
           // if value is false - current step is repeated
       })->addTransition([user,&bot](const Message& m){
           user->password = m.text.value();
           bot.reply(m,"Send your date of birth");
       })->addCheck([&](const Message& m){
           if (!m.text || m.text->size() < 3) {
               bot.reply(m,"Password cannot be empty!");
               return false;
           }
           return true;
       })->addTransition([user,&bot](const Message& m){
           user->date_of_birth = m.text.value();
           bot.sendMessage(m.chat.id,user->to_string());
       })->addCheck([&bot,user](const Message& m){
           if (!m.text || m.text->size() < 3) {
               bot.reply(m,"Date of birth must not be empty.");
               return false;
           }
           return true;
       });
       // you can add a check that will be used to every step
       registration->addCommonCheck([&](const Message& m){
        if (m.text->find("/stop") != std::string::npos){
            bot.reply(m,"Registration cancelled");
            bot.stopSequence(m.chat.id);
            return false;
        };
        return true;
       });
       registration->onExit([&,user](const Message& ){
          // Database::write(user);  // do something with user object
       });
       // add sequence to bot
       bot.startSequence(msg.from->id,registration);
    });
    bot.start(100);
}
```

#### Command bot 
```cpp
#include <telegram_bot.h>

int main() {
    using namespace telegram;
    Bot bot{BOT_TOKEN};
    // or you can you bot.onMessage()
    bot.onEvent<MessageCallback>("/start",[&](const Message& m){
       bot.sendMessage(m.chat.id,"This is bot that can reply to some commands.");
    });
    bot.onEvent<MessageCallback>("/help",[&](const Message& m){
        bot.sendMessage(m.chat.id,"List of allowed commands:\n\t/help\n\t/start\n\t/number\n");
    });
    bot.onEvent<MessageCallback>("/number",[&](const Message& m){
        bot.sendMessage(m.chat.id,std::to_string(rand() % 150));
    });
    // regexes have the lowest priority
    // so the bot will respons this callback if and only
    // if it will not match any callback or command
    bot.onEvent<MessageCallback>(std::regex{"(.*)"},[&](const Message& m){
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

## Documentantion
To generate documentation for project use doxygen 
```
cd /path/to/project
doxygen Doxyfile
```
## Feedback

If you experience any bug or shortcoming, please leave an issue on GitHub or contact me on Telegram (@Malbu0698)

## License

The project is under MIT license so it is free for usage. 



