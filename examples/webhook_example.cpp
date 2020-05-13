#include <telegram_bot.h>

int main() {
    using namespace telegram;
    Bot bot{BOT_TOKEN};

    bot.onUpdate([&](const Update& update){
       if (update.message) {
           bot.sendMessage(update.message->chat.id,"Hello from server!");
       }
    });
    // if you are under NAT you must set up porward forwarding
    bot.setWebhookServer("192.168.0.1",8443); // allowed ports are 8443,443,80,88
}
