#include <gtest/gtest.h>
#include "telegram_bot.h"
using namespace telegram;
TEST(QueryBuilder,builder_builtin) {
    QueryBuilder builder;
    int b = 5;
    int c = 6;
    bool d = false;
    builder << make_named_pair(b) << make_named_pair(c) << make_named_pair(d);
    std::string json = builder.getQuery();
    std::string expected = "{\"b\":5,\"c\":6,\"d\":false}";
    EXPECT_EQ(expected,json);
}
TEST(QueryBuilder,builder_variant) {
    QueryBuilder builder;
    using variant_type = std::variant<std::string,int,bool>;

    variant_type var1 = false;
    variant_type var2 = std::string("test");
    variant_type var3 = 19;
    builder << make_named_pair(var1) << make_named_pair(var2) << make_named_pair(var3);

    std::string json = builder.getQuery();
    std::string expected = "{\"var1\":false,\"var2\":\"test\",\"var3\":19}";
    EXPECT_EQ(expected,json);
}
TEST(QueryBuilder,builder_array) {
    QueryBuilder builder;

    std::vector<int> vec{1,2,3,4};
    builder << make_named_pair(vec);
    std::string json = builder.getQuery();
    std::string expected = "{\"vec\":[1,2,3,4]}";
    EXPECT_EQ(expected,json);
}
struct MetaStruct {
    declare_struct
    declare_field(int,i);
};

TEST(QueryBuilder,builder_array_complex) {
    QueryBuilder builder;

    std::vector<MetaStruct> vec{{1},{2},{3}};
    builder << make_named_pair(vec);

    std::string json = builder.getQuery();
    std::string expected = "{\"vec\":[{\"i\":1},{\"i\":2},{\"i\":3}]}";
    EXPECT_EQ(expected,json);
}
TEST(QueryBuilder,builder_empty_document) {
    QueryBuilder builder;
    std::string json = builder.getQuery();
    std::string expected = "null";
    EXPECT_EQ(expected,json);

}
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
