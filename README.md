# Yet another Telegram Bot Library for C++

Library for Telegram Bot Api written in C++ 

## Requirements

Compiler with C++17\
CMake v 3.5 or higher 

### Build instructions
```
cd /folder/to/lib\
cmake .\
make\
make install\
```
To add library to your project use folliwing CMake command\
`target_link_libraries(project tglib)`\

## Running the tests
To build test for testing Telegram API you must set `BOT_TOKEN` and `TEST_CHAT` variablies \
First one is your bot token, you can get it from `@BotFather` bot in Telegram\
Second one is chat id, where bot will send messages to test (see guide about how to obtain chat id https://stackoverflow.com/ questions/32423837/telegram-bot-how-to-get-a-group-chat-id) \

### Building tests
```
cd /path/to/folder
make test
```
Or you can build them manually, one by one. \

## Examples

There is some examples of bots in `examples` folder. 
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

#### Webhook 
```cpp
#include <telegram_bot.h>

int main() {
    using namespace telegram;
    Bot bot{BOT_TOKEN};

    bot.onUpdate([&](Update&& update){
       if (update.message) {
           bot.sendMessage(update.message->chat.id,"Hello from server!");
       }
    });
    // if you are under NAT set up porward forwarding
    bot.setWebhookServer("192.168.0.1",8443); // allowed ports are 8443,443,80,88
}

```
See more in `examples` folder. \

### In future

1. Builtin ORM support
2. Get all literals removed/replaced by enums 
3. More intuitive sequences 
4. Decreasing compilation speed 
5. Packet managers support

## License

The project is under MIT license so it is free for usage. 



