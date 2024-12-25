#include <gtest/gtest.h>
#include <ulib/json.h>

TEST(JsonTree, AssignAndGet)
{
    {
        ulib::json obj;
        obj.assign("hello");
        ASSERT_EQ(obj.get<ulib::string>(), "hello");
    }

    {
        ulib::json obj;
        obj.assign(30);
        ASSERT_EQ(obj.get<int>(), 30);

        obj.assign(30.5f);
        float gv = obj.get<float>();
        ASSERT_TRUE(gv >= 30.4f && gv <= 30.6f);
    }
}

TEST(JsonTree, Object)
{
    {
        ulib::json obj;
        obj["port"] = 25005;
        obj["text"] = "test";

        ASSERT_EQ(obj["port"].get<int>(), 25005);
        ASSERT_EQ(obj["text"].get<ulib::string>(), "test");
    }
}

TEST(JsonTree, NestedObject)
{
    {
        ulib::json obj;
        obj["object"]["port"] = 25005;
        obj["object"]["text"] = "test";

        ASSERT_EQ(obj["object"]["port"].get<int>(), 25005);
        ASSERT_EQ(obj["object"]["text"].get<ulib::string>(), "test");
    }
}

TEST(JsonTree, Search)
{
    ulib::json value;
    value["hellword"] = "test";
    value["option"] = "check";

    auto v = value.search("hellword");
    ASSERT_TRUE(v != nullptr);
    ASSERT_TRUE(v->get<ulib::string>() == "test");

    // ASSERT_EQ(value.dump(), "\"full\\nplak\"");
}

TEST(JsonTree, EncodingsInConstruct)
{
    {
        ulib::json value = "hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = u8"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = u"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = U"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = L"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = (ulib::string) "hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = (ulib::u8string)u8"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = (ulib::u16string)u"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = (ulib::u32string)U"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = (ulib::wstring)L"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = (std::string) "hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = (std::u16string)u"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = (std::u32string)U"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = (std::wstring)L"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = (ulib::string_view) "hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = (ulib::u8string_view)u8"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = (ulib::u16string_view)u"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = (ulib::u32string_view)U"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = (ulib::wstring_view)L"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = (std::string_view) "hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = (std::u8string_view)u8"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = (std::u16string_view)u"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = (std::u32string_view)U"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value = (std::wstring_view)L"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    // ASSERT_EQ(value.dump(), "\"full\\nplak\"");
}

TEST(JsonTree, EncodingsInAssignment)
{
    {
        ulib::json value;
        value = "hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = u8"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = u"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = U"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = L"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = (ulib::string) "hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = (ulib::u8string)u8"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = (ulib::u16string)u"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = (ulib::u32string)U"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = (ulib::wstring)L"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = (std::string) "hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = (std::u16string)u"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = (std::u32string)U"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = (std::wstring)L"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = (ulib::string_view) "hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = (ulib::u8string_view)u8"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = (ulib::u16string_view)u"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = (ulib::u32string_view)U"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = (ulib::wstring_view)L"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = (std::string_view) "hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = (std::u8string_view)u8"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = (std::u16string_view)u"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = (std::u32string_view)U"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }

    {
        ulib::json value;
        value = (std::wstring_view)L"hello";
        ASSERT_EQ(value.dump(), R"("hello")");
    }
}

TEST(JsonTree, CombineEncodings)
{
    ulib::json value = u8"hello";
    value = U"world";

    ulib::u16string u16str = value.get<ulib::u16string>();
    std::wstring wstr = value.dump<std::wstring>();

    ASSERT_TRUE(u16str == u"world");
    ASSERT_TRUE(wstr == L"\"world\"");
}

TEST(JsonTree, Removing)
{
    ulib::json value;
    value["one"] = "one";
    value["two"] = "two";
    value["three"] = "three";

    ASSERT_TRUE(value.items().size() == 3);
    value.remove("two");
    ASSERT_TRUE(value.items().size() == 2);

    ASSERT_TRUE(value["one"].is_string());
    ASSERT_TRUE(value["two"].is_null());
    ASSERT_TRUE(value["three"].is_string());
}

TEST(JsonTree, Optional)
{
    std::optional<ulib::string> one = "one";
    std::optional<ulib::string> two = std::nullopt;
    std::optional<ulib::string> three = "three";

    ulib::json value;
    value["one"].assign(one);
    value["two"].assign(two);
    value["three"].assign(three);

    ASSERT_TRUE(value["one"].get<ulib::string>() == "one");
    ASSERT_TRUE(value["two"].type() == ulib::json::value_t::null);
    ASSERT_TRUE(value["three"].try_get<ulib::string>() == "three");

    std::optional<ulib::string> four = "four";
    std::optional<ulib::string> five = std::nullopt;
    std::optional<ulib::string> six = "six";

    value["four"] = four;
    value["five"] = five;
    value["six"] = six;

    ASSERT_TRUE(value["four"].get<ulib::string>() == "four");
    ASSERT_TRUE(value["five"].is_null());
    ASSERT_TRUE(value["six"].try_get<ulib::string>() == "six");
}

TEST(JsonTree, Path)
{
    std::optional<std::filesystem::path> one = "one";
    std::optional<std::filesystem::path> two = std::nullopt;
    std::filesystem::path three = "three";

    ulib::json value;
    value["one"].assign(one);
    value["two"].assign(two);
    value["three"].assign(three);

    ASSERT_TRUE(value["one"].get<std::filesystem::path>() == "one");
    ASSERT_TRUE(value["two"].type() == ulib::json::value_t::null);
    ASSERT_TRUE(value["three"].try_get<std::filesystem::path>() == "three");

    std::optional<std::filesystem::path> four = "four";
    std::optional<std::filesystem::path> five = std::nullopt;
    std::filesystem::path six = "six";

    value["four"] = four;
    value["five"] = five;
    value["six"] = six;

    ASSERT_TRUE(value["four"].get<std::filesystem::path>() == "four");
    ASSERT_TRUE(value["five"].is_null());
    ASSERT_TRUE(value["six"].try_get<std::filesystem::path>() == "six");
}