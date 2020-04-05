#include <gtest/gtest.h>
#include "telegram_bot.h"
using namespace telegram;

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
