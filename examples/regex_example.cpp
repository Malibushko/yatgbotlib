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
