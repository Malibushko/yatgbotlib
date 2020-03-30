#include <telegram_bot.h>
struct User {
    std::string username;
    std::string password;
    std::string date_of_birth;
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
    bot.onCommand("/register",[&](Message&& msg){
       bot.sendMessage(msg.chat.id,"Send your username","Markdown");
       auto registration = std::make_shared<sequence<msg_callback>>();
       auto user = std::make_shared<::User>();
       // transition is an action that bot will do after each step

       // check is a function that is called BEFORE next step
       // and must return boolean value, and indicates
       // if value should be passed to next step or not

       registration->add_transition([user,&bot](Message&& m){
           user->username = m.text.value();
           bot.reply(m,"Send your password");
       })->add_check([&](Message&& m){
           if (!m.text || m.text->size() < 3) {
               bot.reply(m,"Username is empty or too short");
               return false;
           }
           return true; // must always return boolean value
           // if value is false - current step is repeated
       })->add_transition([user,&bot](Message&& m){
           user->password = m.text.value();
           bot.reply(m,"Send your date of birth");
       })->add_check([&](Message&& m){
           if (!m.text || m.text->size() < 3) {
               bot.reply(m,"Password cannot be empty!");
               return false;
           }
           return true;
       })->add_transition([user,&bot](Message&& m){
           user->date_of_birth = m.text.value();
           bot.sendMessage(m.chat.id,user->to_string());
       })->add_check([&bot,user](Message&& m){
           if (!m.text || m.text->size() < 3) {
               bot.reply(m,"Date of birth must not be empty.");
               return false;
           }
           return true;
       });

       // you can add a check that will be used to every step
       registration->add_check_common([&](Message&& m){
        if (m.text->find("/stop") != std::string::npos){
            bot.reply(m,"Registration cancelled");
            bot.stopSequence(m.chat.id);
            return false;
        };
        return true;
       });

       // add sequence to bot
       bot.startSequence(msg.chat.id,registration);
    });
    bot.start();
}
