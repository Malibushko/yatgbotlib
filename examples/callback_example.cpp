#include <telegram_bot.h>

int main() {
    using namespace telegram;
    Bot bot{BOT_TOKEN};
    bot.onMessage("/start",[&](const Message& msg){
       bot.sendMessage(msg.chat.id,"Use `/menu` to get the menu","Markdown");
    });
    bot.onMessage("/menu",[&](const Message& msg){
        InlineKeyboardMarkup keyboard{
               {
                {{"Image",{},{},{"image"}}},
                {{"Voice",{},{},{"voice"}}},
                {{"Video",{},{},{"video"}}},
                {{"Audio",{},{},{"audio"}}},
                {{"Emoji",{},{},{"emoji"}}},
               }
        };
        bot.sendMessage(msg.chat.id,"Choose option:",{},{},{},{},keyboard);
    });
    bot.onQueryCallback("image",[&](const CallbackQuery& cb){
       const int64_t chat_id = cb.message.value()->chat.id;
       bot.sendChatAction(chat_id,"upload_photo");
       bot.sendPhoto(chat_id,std::string(RESOURCE_PATH)+"photo.jpg");
    });
    bot.onQueryCallback("voice",[&](const CallbackQuery& cb){
        const int64_t chat_id = cb.message.value()->chat.id;
        bot.sendChatAction(chat_id,"record_audio");
        bot.sendVoice(chat_id,std::string(RESOURCE_PATH)+"voice.ogg");
    });
    bot.onQueryCallback("video",[&](const CallbackQuery& cb){
        const int64_t chat_id = cb.message.value()->chat.id;
        bot.sendChatAction(chat_id,"upload_video");
        bot.sendVideo(chat_id,std::string(RESOURCE_PATH)+"video.mp4");
    });
    bot.onQueryCallback("audio",[&](const CallbackQuery& cb){
        const int64_t chat_id = cb.message.value()->chat.id;
        bot.sendChatAction(chat_id,"upload_audio");
        bot.sendAudio(chat_id,std::string(RESOURCE_PATH)+"audio.mp3");
    });
    bot.onQueryCallback("emoji",[&](const CallbackQuery& cb){
        const int64_t chat_id = cb.message.value()->chat.id;
        int emoji = rand() % 3;
        std::string emoji_str;
        switch (emoji) {
            case 0:
                emoji_str = "🎲";
            break;
        case 1:
            emoji_str = "🎯";
            break;
        default:
            emoji_str = "🏀";
            break;
        }
        bot.sendDice(chat_id,emoji_str);
    });
    bot.start(100);
}
