#include <telegram_bot.h>

int main() {
    using namespace telegram;
    Bot bot{BOT_TOKEN};
    bot.onCommand("/start",[&](Message&& msg){
       bot.sendMessage(msg.chat.id,"Use `/menu` to get the menu","Markdown");
    });
    bot.onCommand("/menu",[&](Message&& msg){
        InlineKeyboardMarkup keyboard{
               {
                {{"Image",{},{},{"image"}}},
                {{"Voice",{},{},{"voice"}}},
                {{"Video",{},{},{"video"}}},
                {{"Audio",{},{},{"audio"}}}
               }
        };
        bot.sendMessage(msg.chat.id,"Choose option:",{},{},{},{},keyboard);
    });
    bot.onCallback("image",[&](CallbackQuery&& cb){
       const int64_t chat_id = cb.message.value().chat.id;
       bot.sendChatAction(chat_id,"upload_photo");
       bot.sendPhoto(chat_id,std::string(RESOURCE_PATH)+"photo.jpg");
    });
    bot.onCallback("voice",[&](CallbackQuery&& cb){
        const int64_t chat_id = cb.message.value().chat.id;
        bot.sendVoice(chat_id,std::string(RESOURCE_PATH)+"voice.ogg");
    });
    bot.onCallback("video",[&](CallbackQuery&& cb){
        const int64_t chat_id = cb.message.value().chat.id;
        bot.sendChatAction(chat_id,"upload_video");
        bot.sendVideo(chat_id,std::string(RESOURCE_PATH)+"video.mp4");
    });
    bot.onCallback("audio",[&](CallbackQuery&& cb){
        const int64_t chat_id = cb.message.value().chat.id;
        bot.sendChatAction(chat_id,"upload_audio");
        bot.sendAudio(chat_id,std::string(RESOURCE_PATH)+"audio.mp3");
    });
    bot.start();
}