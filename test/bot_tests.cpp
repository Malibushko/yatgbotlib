#include "telegram_bot.h"
#include <gtest/gtest.h>
using namespace telegram;
constexpr static std::string_view bot_token = BOT_TOKEN;
constexpr static int64_t chat_id = TEST_CHAT_ID;
Bot bot{bot_token.data()};

TEST(BotTests, getMe) {
  auto &&[result, error] = bot.getMe();
  ASSERT_FALSE(error);
  ASSERT_TRUE(result.id != 0);
  ASSERT_TRUE(result.is_bot);
}
TEST(BotTests, sendMessage) {
  auto &&[result, error] = bot.sendMessage(chat_id, "sendMessage test");
  ASSERT_FALSE(error);
  ASSERT_TRUE(result.message_id);
  ASSERT_TRUE(result.chat.id);
}
TEST(BotTests, sendPhoto) {
  auto &&[result, error] = bot.sendPhoto(
      chat_id, std::string(TEST_PATH) + "photo.jpg", "'sendPhoto'test");
  ASSERT_FALSE(error);
  ASSERT_TRUE(result.chat.id);
  ASSERT_TRUE(result.message_id);
}
TEST(BotTests, sendVideo) {
  auto &&[result, error] =
      bot.sendVideo(chat_id, std::string(TEST_PATH) + "video.mp4");
  ASSERT_FALSE(error);
  ASSERT_TRUE(result.chat.id);
  ASSERT_TRUE(result.message_id);
}
TEST(BotTests, sendAudio) {
  auto &&[result, error] = bot.sendAudio(
      chat_id, std::string(TEST_PATH) + "audio.mp3", "'sendAudio' test");
  ASSERT_FALSE(error);
  ASSERT_TRUE(result.chat.id);
  ASSERT_TRUE(result.message_id);
}
TEST(BotTests, sendDocument) {
  auto &&[result, error] =
      bot.sendDocument(chat_id, std::string(TEST_PATH) + "document.gif", {},
                       "'sendDocument' test");
  ASSERT_FALSE(error);
  ASSERT_TRUE(result.chat.id);
  ASSERT_TRUE(result.message_id);
}
TEST(BotTests, sendAnimation) {
  auto &&[result, error] =
      bot.sendAnimation(chat_id, std::string(TEST_PATH) + "document.gif");
  ASSERT_FALSE(error);
  ASSERT_TRUE(result.chat.id);
  ASSERT_TRUE(result.message_id);
}
TEST(BotTests, sendVoice) {
  auto &&[result, error] = bot.sendVoice(
      chat_id, std::string(TEST_PATH) + "voice.ogg", "'sendAnimation' test");
  ASSERT_FALSE(error);
  ASSERT_TRUE(result.chat.id);
  ASSERT_TRUE(result.message_id);
}

TEST(BotTests, sendVideoNote) {
  auto &&[result, error] =
      bot.sendVideoNote(chat_id, std::string(TEST_PATH) + "videonote.mp4");
  ASSERT_FALSE(error);
  ASSERT_TRUE(result.chat.id);
  ASSERT_TRUE(result.message_id);
}

TEST(BotTests, sendChatAction) {
  auto &&[result, error] = bot.sendChatAction(chat_id, ChatAction::UploadPhoto);
  ASSERT_FALSE(error);
  ASSERT_TRUE(result);
}


TEST(BotTests, sendLocation) {
  auto &&[result, error] = bot.sendLocation(chat_id, 3.55, 1.23);
  ASSERT_FALSE(error);
  ASSERT_TRUE(result.chat.id);
  ASSERT_TRUE(result.message_id);
}

int main(int argc, char **argv) {
  static_assert(!bot_token.empty(), "Bot token is empty");
  static_assert(chat_id != 0, "Chat for testing not set");
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
