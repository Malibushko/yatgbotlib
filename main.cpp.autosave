#include <iostream>
#include <string>
#include <utility>
#include <variant>
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "telegram_structs.h"
#include "telegram_bot.h"
#include <string>

int main() {
    using namespace telegram;

    Bot bot("663528116:AAG-0bX4VLsnkMVzDpA4Ak_8dWMgRquD3ig");
    bot.onUpdate([&](Update && update){
        if (update.message) {
           const auto& msg = update.message.value();
           InlineKeyboardButton link_btn{"Не удаляй меня :(",{},{},"callback"};
           InlineKeyboardMarkup keyboard {{{link_btn}}};
           auto&& [reply,error] = bot.sendPhoto(msg.chat.id,"https://www.meme-arsenal.com/memes/a8c4f3f987efb480b798df96612c7d9e.jpg");
                   if (error) {
               std::cout << error.value() << std::endl;
           }
        }
    });
    bot.onCommand("/count",[&](Message&& m){
       bot.reply(m,"Один");
       auto seq = std::make_shared<Sequence<msg_callback>>();
       seq->addTransition([&bot](Message&& m){
          bot.reply(m,"1");
       })->addTransition([&bot](Message&& m){
           bot.reply(m,"2");
        })->addTransition([&bot](Message&& m){
           bot.reply(m,"3");
        })->addTransition([&bot](Message&& m){
           bot.reply(m,"4");
        })->addTransition([&bot](Message&& m){
           bot.reply(m,"5");
        })->addTransition([&bot](Message&& m){
           bot.reply(m,"Всё!");
        });
       bot.startSequence(m.from->id,seq);
    });
    bot.onCommand("/reg",[&](Message&& m){
        bot.reply(m,"Как тебя зовут?");
        auto seq = std::make_shared<Sequence<msg_callback>>();
        struct User {
            std::string name;
            std::string surname;
        };
        auto user = std::make_shared<User>();
        seq->addTransition([user,&bot](Message && m) {
            user->name = m.text.value();
            bot.reply(m,"Какая у тебя фамилия?");
        })->addCheck([&bot](Message&& m){
            if (!m.text || m.text.value().size() < 3) {
                bot.reply(m,"Слишком маленькое имя.");
                return false;
            }
            return true;
        })->addTransition([user,&bot](Message&& m) {
            user->surname = m.text.value();
            bot.reply(m,"Привет, " + user->name + " " + user->surname + "!");
        })->addCheck([user,&bot](Message&& m){
            if (!m.text || m.text.value().size() < 3) {
                bot.reply(m,"Сликом маленькая фамилия.");
                return false;
            }
            return true;
        });
        seq->addCommonCheck([&bot,seq](Message && m){
            if (m.text == "/cancel") {
                bot.reply(m,"Регистрация отменена");
                seq->finish();
                return false;
            }
            return true;
        });
        bot.startSequence(m.from->id,seq);
    });
    bot.onCallback("callback",[&](CallbackQuery&& q){
        bot.deleteMessage(q.message->chat.id,q.message->message_id);  
    });
    bot.onCommand("/help",[&](Message&& m){
       bot.sendMessage(m.chat.id,"Бог тебе поможет, аминь");
    });
    bot.onCommand("/helper",[&](Message&& m){
        bot.sendMessage(m.chat.id,"Ого егор ахуенно сделал");
    });
    auto&& [reply,error] = bot.deleteWebhook();
            std::cout << reply;
            std::cout << std::endl;
    constexpr uint16_t telegram_supported_port = 8443;
   // bot.setWebhookServer("194.67.108.138",telegram_supported_port);
    bot.start();
}


