#include <gtest/gtest.h>
#include "telegram_bot.h"
using namespace telegram;
struct Small {
    declare_struct;
    declare_field(bool,test);
};
TEST(JsonParser,to_json_small_structure) {
    std::string json = to_json(Small{false});
    std::string expected_json = "{\"test\":false}";
    EXPECT_EQ(expected_json,json);
}
struct Large {
    declare_struct;
    declare_field(bool,b1);
    declare_field(bool,b2);
    declare_field(bool,b3);
    declare_field(bool,b4);
    declare_field(bool,b5);
    declare_field(bool,b6);
    declare_field(bool,b7);
    declare_field(bool,b8);
    declare_field(bool,b9);
    declare_field(bool,b10);
    declare_field(bool,b11);
    declare_field(bool,b12);
    declare_field(bool,b13);
    declare_field(bool,b14);
    declare_field(bool,b15);
    declare_field(bool,b16);
    declare_field(bool,b17);
    declare_field(bool,b18);
    declare_field(bool,b19);
    declare_field(bool,b20);
    declare_field(bool,b21);
    declare_field(bool,b22);
    declare_field(bool,b23);
    declare_field(bool,b24);
    declare_field(bool,b25);
    Large() = default;
};
TEST(JsonParser,to_json_large_structure) {
    std::string json = to_json(Large{});
    std::string expected_json = "{"
        "\"b\":\"false\","
                                 "\"b1\":\"false\","
                                 "\"b2\":\"false\","
                                 "\"b3\":\"false\","
                                 "\"b4\":\"false\","
                                 "\"b5\":\"false\","
                                 "\"b6\":\"false\","
                                 "\"b7\":\"false\","
                                 "\"b8\":\"false\","
                                 "\"b9\":\"false\","
                                 "\"b10\":\"false\","
                                 "\"b11\":\"false\","
                                 "\"b12\":\"false\","
                                 "\"b13\":\"false\","
                                 "\"b14\":\"false\","
                                 "\"b15\":\"false\","
                                 "\"b16\":\"false\","
                                 "\"b17\":\"false\","
                                 "\"b18\":\"false\","
                                 "\"b19\":\"false\","
                                 "\"b20\":\"false\","
                                 "\"b21\":\"false\","
                                 "\"b22\":\"false\","
                                 "\"b23\":\"false\","
                                 "\"b24\":\"false\","
                                 "\"b25\":\"false\""
                                "}";
    EXPECT_EQ(expected_json,json);
}
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
