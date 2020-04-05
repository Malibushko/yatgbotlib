#include <telegram_structs.h>
#include <iostream>
#include <boost/pfr.hpp>
#include <telegram_bot.h>
#include "headers/dbmanager.h"

struct User {
    define_struct(User)
    define_field(std::string,username);
    define_field(std::string,password);
    define_field(std::string,date_of_birth);
    std::string to_string() {
        std::string buffer;
        const int literal_size = 34; // sum of string literals
        buffer.reserve(username.size()+password.size()+date_of_birth.size()+literal_size);
        buffer += "\nName: " + username +
                  "\nPassword: " + password +
                  "\nBirth date: " + date_of_birth + '\n';
        return buffer;
    }
};

int main() {
    using namespace telegram;
    Bot bot{BOT_TOKEN};
    DatabaseManager<::User> db("./users.sqlite");

    bot.onCommand("/register",[&](Message&& msg){
       bot.sendMessage(msg.chat.id,"Send your username",ParseMode::Markdown);
       auto registration = std::make_shared<Sequence<MessageCallback>>();
       auto user = std::make_shared<::User>();
       // transition is an action that bot will do after each step

       // check is a function that is called BEFORE next step
       // and must return boolean value, and indicates
       // if value should be passed to next step or not

       registration->addTransition([user,&bot](Message&& m){
           user->username = m.text.value();
           bot.reply(m,"Send your password");
       })->addCheck([&](Message&& m){
           if (!m.text || m.text->size() < 3) {
               bot.reply(m,"Username is empty or too short");
               return false;
           }
           return true; // must always return boolean value
           // if value is false - current step is repeated
       })->addTransition([user,&bot](Message&& m){
           user->password = m.text.value();
           bot.reply(m,"Send your date of birth");
       })->addCheck([&](Message&& m){
           if (!m.text || m.text->size() < 3) {
               bot.reply(m,"Password cannot be empty!");
               return false;
           }
           return true;
       })->addTransition([user,&bot](Message&& m){
           user->date_of_birth = m.text.value();
           bot.sendMessage(m.chat.id,user->to_string());
       })->addCheck([&bot,user](Message&& m){
           if (!m.text || m.text->size() < 3) {
               bot.reply(m,"Date of birth must not be empty.");
               return false;
           }
           return true;
       });

       // you can add a check that will be used to every step
       registration->addCommonCheck([&](Message&& m){
        if (m.text->find("/stop") != std::string::npos){
            bot.reply(m,"Registration cancelled");
            bot.stopSequence(m.chat.id);
            return false;
        };
        return true;
       });
       registration->onExit([&,user](Message&& m){
           db.instance().insert(*user);
           bot.sendMessage(m.chat.id,"Succesfully registered!");
       });
       // add sequence to bot
       bot.startSequence(msg.from->id,registration);
    });
    bot.onCommand("/login",[&](Message&& msg){
        auto login = std::make_shared<Sequence<MessageCallback>>();
        std::shared_ptr<::User> user = std::make_shared<::User>();
        bot.reply(msg,"Enter login:");
        login->addTransition([&,login,user](Message&& m){
            user->username = m.text.value_or("");
            bot.reply(m,"Enter you password:");
        })->addTransition([&,login,user](Message&& m){
            user->password = m.text.value_or("");
            using namespace sqlite_orm;
            auto result = db.instance().select(columns(&::User::username,&::User::password),
                                             where(is_equal(&::User::username,user->username
                                                            && is_equal(&::User::password,user->password))));
            if (result.size())
                bot.reply(m,"Successfully logged in");
            else
                bot.reply(m,"User not found.");
        });
        bot.startSequence(msg.from->id,login);
    });
    bot.start(100);
}

