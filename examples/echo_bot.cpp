#include <telegram_bot.h>

int main() {
    using namespace telegram;
    Bot bot{BOT_TOKEN};
    bot.onUpdate([&](const Update& update){
       if (update.message) {
           bot.reply(update.message.value(),update.message.value().text.value_or("<no text>"));
       }
    });
    bot.start(100);
}
