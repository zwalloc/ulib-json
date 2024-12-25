#include <gtest/gtest.h>
#include <ulib/json.h>

TEST(Tree, CanParseString)
{
    auto value = ulib::json::parse(R"("hello")");

    ASSERT_TRUE(value.type() == ulib::json::value_t::string);
    ASSERT_EQ(value.get<ulib::string>(), "hello");
}

TEST(Tree, CanParseInt)
{
    auto value = ulib::json::parse("2600");

    ASSERT_TRUE(value.is_int());
    ASSERT_EQ(value.get<int>(), 2600);
}

TEST(Tree, CanParseFloat)
{
    auto value = ulib::json::parse("2600.5");
    ASSERT_TRUE(value.is_float());

    float flt = value.get<float>();
    ASSERT_TRUE(flt >= 2600.4f && flt <= 2600.6f);
}

TEST(Tree, CanParseBool)
{
    auto true_value = ulib::json::parse("true");

    ASSERT_TRUE(true_value.is_bool());
    ASSERT_EQ(true_value.get<bool>(), true);

    auto false_value = ulib::json::parse("false");

    ASSERT_TRUE(false_value.is_bool());
    ASSERT_EQ(false_value.get<bool>(), false);
}


TEST(Tree, CanParseIntArray)
{
    auto value = ulib::json::parse(R"([250, 132, 600])");

    ASSERT_TRUE(value.is_array());

    ASSERT_EQ(value.at(0).get<int>(), 250);
    ASSERT_EQ(value.at(1).get<int>(), 132);
    ASSERT_EQ(value.at(2).get<int>(), 600);

    ASSERT_TRUE(value[0].is_int());
    ASSERT_TRUE(value[1].is_int());
    ASSERT_TRUE(value[2].is_int());

    ASSERT_EQ(value[0].get<int>(), 250);
    ASSERT_EQ(value[1].get<int>(), 132);
    ASSERT_EQ(value[2].get<int>(), 600);
}

TEST(Tree, CanParseStringArray)
{
    auto value = ulib::json::parse(R"(["hello", "hi", "hey whats up"])");

    ASSERT_TRUE(value.is_array());

    ASSERT_TRUE(value[0].is_string());
    ASSERT_TRUE(value[1].is_string());
    ASSERT_TRUE(value[2].is_string());

    ASSERT_EQ(value[0].get<std::string>(), "hello");
    ASSERT_EQ(value[1].get<std::string>(), "hi");
    ASSERT_EQ(value[2].get<std::string>(), "hey whats up");
}

TEST(Tree, CanParseObject)
{
    auto value = ulib::json::parse(R"({"key": "value"})");

    ASSERT_EQ(value["key"].get<std::string>(), "value");
}

TEST(Tree, CanParseStringWithSpaces)
{
    auto parse = [](std::string_view str) {
        auto value = ulib::json::parse(std::string(str));

        ASSERT_TRUE(value.is_string());
        ASSERT_EQ(value.get<std::string>(), "hello");
    };

    parse(R"( "hello" )");
    parse(R"( "hello")");
}

TEST(Tree, CanParseIntWithSpaces)
{
    auto parse = [](std::string_view str) {
        auto value = ulib::json::parse(std::string(str));

        ASSERT_TRUE(value.is_int());
        ASSERT_EQ(value.get<int>(), 2600);
    };

    parse(" 2600");
    parse(" \n 2600 \n ");
}

TEST(Tree, CanParseFloatWithSpaces)
{
    auto parse = [](std::string_view str) {
        auto value = ulib::json::parse(std::string(str));
        ASSERT_TRUE(value.is_float());

        float flt = value.get<float>();
        ASSERT_TRUE(flt >= 2600.4f && flt <= 2600.6f);
    };

    parse(" 2600.5");
    parse(" \n 2600.5 \n ");
}

TEST(Tree, CanParseBoolWithSpaces)
{
    auto parse_true = [](std::string_view str) {
        auto true_value = ulib::json::parse(std::string(str));

        ASSERT_TRUE(true_value.is_bool());
        ASSERT_EQ(true_value.get<bool>(), true);
    };

    auto parse_false = [](std::string_view str) {
        auto false_value = ulib::json::parse(std::string(str));

        ASSERT_TRUE(false_value.is_bool());
        ASSERT_EQ(false_value.get<bool>(), false);
    };

    parse_true(" true");
    parse_true(" \n true \n ");

    parse_false(" false");
    parse_false(" \n false \n ");
}

TEST(Tree, CanParseIntArrayWithSpaces)
{
    auto parse = [](std::string_view str) {
        auto value = ulib::json::parse(std::string(str));

        ASSERT_TRUE(value.is_array());

        ASSERT_EQ(value.at(0).get<int>(), 250);
        ASSERT_EQ(value.at(1).get<int>(), 132);
        ASSERT_EQ(value.at(2).get<int>(), 600);

        ASSERT_TRUE(value[0].is_int());
        ASSERT_TRUE(value[1].is_int());
        ASSERT_TRUE(value[2].is_int());

        ASSERT_EQ(value[0].get<int>(), 250);
        ASSERT_EQ(value[1].get<int>(), 132);
        ASSERT_EQ(value[2].get<int>(), 600);
    };

    parse(R"( [250, 132, 600] )");
    parse(R"([250, 132, 600] )");
    parse(R"( [250, 132, 600])");
    parse(R"(
        [  250
        , 132  , 600]
        )");
}

TEST(Tree, ParseObjectWithArrayWithOneString)
{
    auto value = ulib::json::parse(R"(
        {
            "options": ["norepeat"]
        }
    )");

    ASSERT_TRUE(value.is_object());
    ASSERT_NO_THROW(value.at("options"));

    auto options = value.at("options");
    ASSERT_TRUE(options.is_array());
    ASSERT_NO_THROW(options.at(0));

    auto v0 = options.at(0);
    ASSERT_TRUE(v0.is_string());
    ASSERT_TRUE(v0.get<std::string>() == "norepeat");
}

TEST(Tree, ParseEmptyArray)
{
    auto value = ulib::json::parse(R"([])");

    ASSERT_TRUE(value.is_array());
    ASSERT_TRUE(value.values().size() == 0);
}

TEST(Tree, ParseEmptyObject)
{
    auto value = ulib::json::parse(R"({})");

    ASSERT_TRUE(value.is_object());
    ASSERT_TRUE(value.items().size() == 0);
}

TEST(Tree, ParseEmptyString)
{
    auto value = ulib::json::parse(R"("")");

    ASSERT_TRUE(value.is_string());
    ASSERT_TRUE(value.get<ulib::string_view>().size() == 0);
}


TEST(Tree, ParseNull)
{
    auto value = ulib::json::parse(R"(null)");
    ASSERT_TRUE(value.is_null());
}