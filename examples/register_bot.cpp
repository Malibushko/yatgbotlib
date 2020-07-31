#include <telegram_bot.h>
struct User {
    std::string username;
    std::string password;
    std::string date_of_birth;
    std::string to_string() {
        std::string buffer;
        buffer.reserve(username.size()+password.size()+date_of_birth.size());
        buffer += "\nName: " + username +
                  "\nPassword: " + password +
                  "\nBirth date: " + date_of_birth + '\n';
        return buffer;
    }
};

int main() {
    using namespace telegram;
    Bot bot{BOT_TOKEN};
    bot.onMessage("/register",[&](const Message& msg){
       bot.sendMessage(msg.chat.id,"Send your username","Markdown");
       auto registration = std::make_shared<Sequence<MessageCallback>>();
       auto user = std::make_shared<::User>();
       // transition is an action that bot will do after each step

       // check is a function that is called BEFORE next step
       // and must return boolean value, and indicates
       // if value should be passed to next step or not

       registration->addTransition([user,&bot](const Message& m){
           user->username = m.text.value();
           bot.reply(m,"Send your password");
       })->addCheck([&](const Message& m){
           if (!m.text || m.text->size() < 3) {
               bot.reply(m,"Username is empty or too short");
               return false;
           }
           return true; // must always return boolean value
           // if value is false - current step is repeated
       })->addTransition([user,&bot](const Message& m){
           user->password = m.text.value();
           bot.reply(m,"Send your date of birth");
       })->addCheck([&](const Message& m){
           if (!m.text || m.text->size() < 3) {
               bot.reply(m,"Password cannot be empty!");
               return false;
           }
           return true;
       })->addTransition([user,&bot](const Message& m){
           user->date_of_birth = m.text.value();
           bot.sendMessage(m.chat.id,user->to_string());
       })->addCheck([&bot,user](const Message& m){
           if (!m.text || m.text->size() < 3) {
               bot.reply(m,"Date of birth must not be empty.");
               return false;
           }
           return true;
       });
       // you can add a check that will be used to every step
       registration->addCommonCheck([&](const Message& m){
        if (m.text->find("/stop") != std::string::npos){
            bot.reply(m,"Registration cancelled");
            bot.stopSequence(m.chat.id);
            return false;
        };
        return true;
       });
       registration->onExit([&,user](const Message& ){
          // Database::write(user);  // do something with user object
       });
       // add sequence to bot
       bot.startSequence(msg.from->id,registration);
    });
    bot.start(100);
}
