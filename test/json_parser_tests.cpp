#include <gtest/gtest.h>
#include <memory>
#include "telegram_bot.h"
using namespace telegram;
struct Small {
    declare_struct;
    declare_field(bool,test);
};

TEST(JsonParser,to_json_small_structure) {
    std::string json = JsonParser::i().toJson(Small{false});
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
    std::string json = JsonParser::i().toJson(Large{});
    std::string expected_json = "{"
                                "\"b1\":false,"
                                "\"b2\":false,"
                                "\"b3\":false,"
                                "\"b4\":false,"
                                "\"b5\":false,"
                                "\"b6\":false,"
                                "\"b7\":false,"
                                "\"b8\":false,"
                                "\"b9\":false,"
                                "\"b10\":false,"
                                "\"b11\":false,"
                                "\"b12\":false,"
                                "\"b13\":false,"
                                "\"b14\":false,"
                                "\"b15\":false,"
                                "\"b16\":false,"
                                "\"b17\":false,"
                                "\"b18\":false,"
                                "\"b19\":false,"
                                "\"b20\":false,"
                                "\"b21\":false,"
                                "\"b22\":false,"
                                "\"b23\":false,"
                                "\"b24\":false,"
                                "\"b25\":false"
                                "}";
    EXPECT_EQ(expected_json,json);
}
struct Array {
    declare_struct
    declare_field(std::vector<int>,data);
};

TEST(JsonParser,to_json_array_type) {
    Array arr;
    arr.data = {1,2,3,4,5};
    std::string json = JsonParser::i().toJson(arr);
    std::string expected_json = "{\"data\":[1,2,3,4,5]}";
    EXPECT_EQ(expected_json,json);

}
struct ComplexArray {
    declare_struct
    declare_field(std::vector<Small>,data);
};

TEST(JsonParser,to_json_complex_array_type) {
    ComplexArray arr;
    arr.data = {{false},{true},{false}};
    std::string json = JsonParser::i().toJson(arr);
    std::string expected_json = "{\"data\":[{\"test\":false},{\"test\":true},{\"test\":false}]}";
    EXPECT_EQ(expected_json,json);

}

struct SharedPtr {
    declare_struct
    declare_field(std::shared_ptr<int>,data);
};

TEST(JsonParse,to_json_unique_ptr) {
    SharedPtr ptr;
    ptr.data = std::make_unique<int>(5);
    std::string json = JsonParser::i().toJson(ptr);
    std::string expected_json = "{\"data\":5}";
    EXPECT_EQ(expected_json,json);
}
struct Variant {
    using variant_type = std::variant<std::string,int>;
    declare_struct
    declare_field(variant_type,data);
};

TEST(JsonParser,to_json_variant) {
    Variant test1;
    Variant test2;
    test1.data = 5;
    test2.data = "Test";
    std::string json_1 = JsonParser::i().toJson(test1);
    std::string expected_json_1 = "{\"data\":5}";

    std::string json_2 = JsonParser::i().toJson(test2);
    std::string expected_json_2 = "{\"data\":\"Test\"}";

    EXPECT_EQ(expected_json_1,json_1);
    EXPECT_EQ(expected_json_2,json_2);
}
struct Matrix {
    declare_struct;
    declare_field(std::vector<std::vector<int>>,data);
};

TEST(JsonParser,to_json_2darray) {
    Matrix m{
            {{1,2,3},
            {4,5,6},
            {7,8,9}}
    };
    std::string json = JsonParser::i().toJson(m);
    std::string expected_json = "{\"data\":[[1,2,3],[4,5,6],[7,8,9]]}";
    EXPECT_EQ(expected_json,json);
}
TEST(JsonParser,forward_reverse_parse) {
    Small m{true};
    std::string json = JsonParser::i().toJson(m);
    Small after = JsonParser::i().fromJson<Small>(json);
    EXPECT_EQ(m.test,after.test);
}
TEST(JsonParser,parse_array) {
    ComplexArray arr = JsonParser::i().fromJson<ComplexArray>({"{\"data\":[{\"test\":false},{\"test\":true},{\"test\":false}]}"});
    auto expected = std::vector<Small>{{false},{true},{false}};
    if (arr.data.size() != expected.size())
        ASSERT_TRUE(false);

    for (size_t i = 0; i < arr.data.size();++i)
        if (arr.data[i].test != expected[i].test) {
            ASSERT_TRUE(false);
            break;
        }
    ASSERT_TRUE(true);
}
TEST(JsonParse,parse_unique_ptr) {
    SharedPtr ptr = JsonParser::i().fromJson<SharedPtr>({"{\"data\":5}"});
    if (!ptr.data)
        ASSERT_TRUE(false);
    EXPECT_EQ((*ptr.data),5);
}
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
