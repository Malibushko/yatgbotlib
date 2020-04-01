#include <telegram_bot.h>
#include <array>
using namespace telegram;

// generate random number of size N in form of array
template <size_t N>
std::array<int,N> getRandomNumber() {
    std::array<int,N> password;
    std::random_device d;
    std::mt19937 gen(d());
    std::uniform_int_distribution<> distr(1,9);

    std::generate(password.begin(),password.end(),[&](){return distr(gen);});
    return password;
}
// create keyboard in a form of matrix
InlineKeyboardMarkup createKeyboard() {
    InlineKeyboardMarkup keyboard;
    keyboard.inline_keyboard.resize(3);
    for (size_t i = 0; i < 3;++i) {
        keyboard.inline_keyboard[i].resize(3);
        for (size_t j = 0; j < 3;++j) {
            auto string_counter = std::to_string(3*i+j+1);
            keyboard.inline_keyboard[i][j] = InlineKeyboardButton{string_counter,{},{},string_counter};
        }
    }
    return keyboard;
}
int main() {
    using namespace telegram;
    Bot bot{BOT_TOKEN}; // set BOT_TOKEN in CMake file
    bot.onCommand("/login",[&](Message&& msg){
        auto password = getRandomNumber<5>();
        std::stringstream ss;
        for (auto it : password)
            ss << it;
        // create string out of array
        auto str = ss.str();
        bot.sendMessage(msg.chat.id,"Enter `" + str + "`\n","Markdown",{},{},{},createKeyboard());

        // create sequence
        auto password_check = std::make_shared<Sequence<QueryCallback>>();
        for (std::size_t i = 0; i < password.size();++i) {
            // add transition for each number
            password_check->addTransition([&bot,&msg,password_check,number = str[i]](CallbackQuery&& q){
                if (!q.data || q.data->at(0) != number) {
                    bot.reply(q.message.value(),"Failed to log in");
                    bot.stopSequence(msg.from->id);
                }
            });
        }
        // if everything is successfull - send user a message
        password_check->onExit([&bot](CallbackQuery&& q){
            bot.reply(q.message.value(),"Successfully logged in!");
        });
        // add message to bot (not thread-safe)
        bot.startSequence(msg.from->id,password_check);
    });
    bot.start();
}
