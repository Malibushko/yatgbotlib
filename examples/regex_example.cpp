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
    bot.onEvent<MessageCallback>(std::regex{"(.*)"},[&](Message&& m){
       bot.sendMessage(m.chat.id,"Command is not supported. See list of supported commantd /h");
    });
    // you can use regexes with any <callback>Callback signature;
    bot.start(100);
}
