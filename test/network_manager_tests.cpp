#include <gtest/gtest.h>
#include "telegram_bot.h"
using namespace telegram;
TEST(NetworkManager,post_telegram_test) {
    auto reply = network_manager::i().post("https://api.telegram.org/bot123456:ABC-DEF1234ghIkl"
                                           "-zyx57W2v1u123ew11/getMe");

    ASSERT_TRUE(reply);
    std::string expected = "{\"ok\":false,\"error_code\":401,\"description\":\"Unauthorized\"}";
    EXPECT_EQ(reply->body,expected);
    EXPECT_EQ(reply->status,401);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
