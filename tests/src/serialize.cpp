#include <gtest/gtest.h>

#include <ulib/json.h>

TEST(Tree, CanSerializeBool)
{
    ASSERT_EQ(ulib::json{true}.dump(), "true");
    ASSERT_EQ(ulib::json{false}.dump(), "false");

    ASSERT_NE(ulib::json{true}.dump(), "false");
    ASSERT_NE(ulib::json{false}.dump(), "true");
}

TEST(Tree, CanSerializeInt)
{
    ASSERT_EQ(ulib::json{2000}.dump(), "2000");
    ASSERT_NE(ulib::json{2000}.dump(), "1500");
}

TEST(Tree, CanSerializeString)
{
    ASSERT_EQ(ulib::json{"fullest"}.dump(), "\"fullest\"");
    ASSERT_NE(ulib::json{"fullest"}.dump(), "\"night\"");
}

TEST(Tree, SerializeArrayWithOneString)
{
    auto value = ulib::json::parse(R"(["hello"])");

    ASSERT_EQ(value.type(), ulib::json::value_t::array);
    ASSERT_EQ(value.size(), 1);

    ASSERT_EQ(value.dump(), R"(["hello"])");
}

TEST(Tree, Serialization)
{
    ulib::json value;
    value[0] = 500;
    value[1] = ulib::json::parse(R"(
        {
            "key": "value",
            "port": 1703,
            "host": "127.0.0.1",
            "options": ["norepeat"],
            "object": { "color": "green" }
        })");

    ASSERT_EQ(value[0].dump(), R"(500)");
    ASSERT_EQ(value[1]["key"].dump(), R"("value")");
    ASSERT_EQ(value[1]["port"].dump(), R"(1703)");
    ASSERT_EQ(value[1]["host"].dump(), R"("127.0.0.1")");
    ASSERT_EQ(value[1]["options"].dump(), R"(["norepeat"])");
    ASSERT_EQ(value[1]["object"].dump(), R"({"color":"green"})");
}

TEST(Tree, SerializationEscape)
{
    {
        ulib::json value0 = "one\ntwo\nthree";
        ulib::json value1 = ulib::json::parse(value0.dump());

        ASSERT_TRUE(value1.dump().contains("\\n"));
        ASSERT_EQ(value0.dump(), value1.dump());
    }

    {
        ulib::json value0 = "one\ttwo\tthree";
        ulib::json value1 = ulib::json::parse(value0.dump());

        ASSERT_TRUE(value1.dump().contains("\\t"));
        ASSERT_EQ(value0.dump(), value1.dump());
    }

    {
        ulib::json value0 = "one\rtwo\rthree";
        ulib::json value1 = ulib::json::parse(value0.dump());

        ASSERT_TRUE(value1.dump().contains("\\r"));
        ASSERT_EQ(value0.dump(), value1.dump());
    }

    {
        ulib::json value0 = "one\"two\"three";
        ulib::json value1 = ulib::json::parse(value0.dump());

        ASSERT_TRUE(value1.dump().contains("\\\""));
        ASSERT_EQ(value0.dump(), value1.dump());
    }

    {
        ulib::json value0 = "one\\two\\three";
        ulib::json value1 = ulib::json::parse(value0.dump());

        ASSERT_TRUE(value1.dump().contains("\\\\"));
        ASSERT_EQ(value0.dump(), value1.dump());
    }

    {
        ulib::json value0 = "one\btwo\bthree";
        ulib::json value1 = ulib::json::parse(value0.dump());

        ASSERT_TRUE(value1.dump().contains("\\b"));
        ASSERT_EQ(value0.dump(), value1.dump());
    }

    {
        ulib::json value0 = "one\ftwo\fthree";
        ulib::json value1 = ulib::json::parse(value0.dump());

        ASSERT_TRUE(value1.dump().contains("\\f"));
        ASSERT_EQ(value0.dump(), value1.dump());
    }
}

TEST(Tree, DumpEmptyTypes)
{
    ASSERT_EQ(ulib::json{}.dump(), "null");
    ASSERT_EQ(ulib::json{ulib::json::value_t::null}.dump(), "null");
    ASSERT_EQ(ulib::json{ulib::json::value_t::object}.dump(), "{}");
    ASSERT_EQ(ulib::json{ulib::json::value_t::array}.dump(), "[]");
    ASSERT_EQ(ulib::json{ulib::json::value_t::boolean}.dump(), "false");
    ASSERT_EQ(ulib::json{ulib::json::value_t::integer}.dump(), "0");
    ASSERT_EQ(ulib::json{ulib::json::value_t::floating}.dump(), "0.000000");
    ASSERT_EQ(ulib::json{ulib::json::value_t::string}.dump(), "\"\"");

    ASSERT_EQ(ulib::json::object().dump(), "{}");
    ASSERT_EQ(ulib::json::array().dump(), "[]");
}