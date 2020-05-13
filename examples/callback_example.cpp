#include <telegram_bot.h>

int main() {
    using namespace telegram;
    Bot bot{BOT_TOKEN};
    bot.onMessage("/start",[&](const Message& msg){
       bot.sendMessage(msg.chat.id,"Use `/menu` to get the menu",ParseMode::Markdown);
    });
    bot.onMessage("/menu",[&](const Message& msg){
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
    bot.onQueryCallback("image",[&](const CallbackQuery& cb){
       const int64_t chat_id = cb.message.value().chat.id;
       bot.sendChatAction(chat_id,ChatAction::UploadPhoto);
       bot.sendPhoto(chat_id,std::string(RESOURCE_PATH)+"photo.jpg");
    });
    bot.onQueryCallback("voice",[&](const CallbackQuery& cb){
        const int64_t chat_id = cb.message.value().chat.id;
        bot.sendChatAction(chat_id,ChatAction::RecordAudio);
        bot.sendVoice(chat_id,std::string(RESOURCE_PATH)+"voice.ogg");
    });
    bot.onQueryCallback("video",[&](const CallbackQuery& cb){
        const int64_t chat_id = cb.message.value().chat.id;
        bot.sendChatAction(chat_id,ChatAction::UploadVideo);
        bot.sendVideo(chat_id,std::string(RESOURCE_PATH)+"video.mp4");
    });
    bot.onQueryCallback("audio",[&](const CallbackQuery& cb){
        const int64_t chat_id = cb.message.value().chat.id;
        bot.sendChatAction(chat_id,ChatAction::UploadAudio);
        bot.sendAudio(chat_id,std::string(RESOURCE_PATH)+"audio.mp3");
    });
    bot.start(100);
}
