#include "telegram_bot.h"
#include <gtest/gtest.h>
using namespace telegram;
#define stringify(x) #x
constexpr std::string_view bot_token = BOT_TOKEN;

const int64_t chat_id = TEST_CHAT_ID;
const std::string api_url =
    std::string("https://api.telegram.org/bot") + bot_token.data() + '/';

ApiManager mng(api_url.data());
int64_t last_message_id = 0;

TEST(ApiManager, post_no_params_test) {
  auto &&[result, error] = mng.ApiCall<User>("getMe");
  if (error)
    ASSERT_TRUE(false);
  else {
    ASSERT_TRUE(result.id);
    ASSERT_TRUE(result.first_name.size());
  }
}
TEST(ApiManager, post_with_params_test) {
  QueryBuilder builder;
  std::string text = "test";
  builder << make_named_pair(chat_id) << make_named_pair(text);

  auto &&[result, error] = mng.ApiCall<Message>("sendMessage", builder);
  if (error.has_value()) {
    ASSERT_TRUE(false);
  } else {
    ASSERT_TRUE(result.text->size());
    ASSERT_TRUE(result.chat.id);
    last_message_id = result.message_id;
  }
}

TEST(ApiManager, post_conditional) {
  QueryBuilder builder;
  int64_t message_id = last_message_id;
  std::string text = "test successfull";
  builder << make_named_pair(chat_id) << make_named_pair(message_id)
          << make_named_pair(text);

  auto &&[result, error] = mng.ApiCall<std::variant<bool, Message>, Message>(
      "editMessageText", builder);
  if (error)
    ASSERT_TRUE(false);
  else {
    std::visit(
        [](auto &&value) {
          using T = std::decay_t<decltype(value)>;
          if constexpr (!std::is_same_v<T, Message>)
            ASSERT_TRUE(false);
          else {
            ASSERT_TRUE(value.message_id);
          }
        },
        result);
  }
}
int main(int argc, char **argv) {
  static_assert(!bot_token.empty(), "Bot token is empty");
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
