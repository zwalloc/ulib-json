#include "json.h"

#include <fops/i64toa_10_inl.h>

namespace ulib
{
    namespace json_detail
    {
        using StringViewT = typename json::StringViewT;
        using value_t = typename json::value_t;

        size_t serialized_object_length(const json &obj);
        size_t serialized_array_length(const json &obj);
        size_t serialized_length(const json &obj);

        size_t serialized_object_length(const json &obj)
        {
            size_t result = 2; // {}

            auto items = obj.items();
            if (!items.empty())
            {
                for (auto it = items.begin();;)
                {
                    result += serialized_length(it->value()) + it->name().size() + 3; // 2 is first '"' and ':'

                    it++;
                    if (it != items.end())
                        result++; // ,
                    else
                        break;
                }
            }

            return result;
        }

        size_t serialized_array_length(const json &obj)
        {
            size_t result = 2; // []
            auto arr = obj.values();
            if (arr.size())
            {
                for (auto it = arr.begin();;)
                {
                    result += serialized_length(*it);
                    it++;
                    if (it != arr.end())
                    {
                        result++;
                    }
                    else
                    {
                        break;
                    }
                }
            }

            return result;
        }

        size_t serialized_length(const json &obj)
        {
            size_t result = 0;
            int64_t x, n;

            switch (obj.type())
            {
            case value_t::integer:
                x = obj.get<int64_t>();
                n = x < 0 ? 2 : 1; // check on '-'
                while ((x /= 10) != 0)
                    n++;
                result += n;
                break;

            case value_t::floating:
                result += std::to_string(obj.get<float>()).length();
                break;

            case value_t::string:

            {
                auto view = obj.get<ulib::string_view>();

                for (const char *it = view.begin().raw(); it != view.end().raw(); it++)
                {
                    if (*it == '\\' || *it == '\"' || *it == '\n' || *it == '\r' || *it == '\t' || *it == '\b' ||
                        *it == '\f')
                    {
                        result += 2;
                    }
                    else
                    {
                        result++;
                    }
                }
            }

                result += 2; // first and last '\"'

                // result += obj.size() + 1; // 1 is '\"' teriminator
                break;

            case value_t::object:
                result += serialized_object_length(obj);
                break;

            case value_t::array:
                result += serialized_array_length(obj);
                break;

            case value_t::boolean:
                result += obj.get<bool>() ? sizeof("true") - 1 : sizeof("false") - 1;
                break;

            default:
                result += 4; // null
                break;
            }

            return result;
        }

        char *c_serialize(const json &obj, char *_out);
        char *c_serialize_object(const json &obj, char *_out)
        {
            char *out = _out;
            *out = '{';
            out++;

            auto objit = obj.items();
            if (objit.size())
            {
                for (auto it = objit.begin();;)
                {
                    *out = '\"';
                    out++;
                    size_t len = it->name().size();
                    memcpy(out, it->name().data(), len);
                    out += len;
                    *out = '\"';
                    out++;
                    *out = ':';
                    out++;

                    out = c_serialize(it->value(), out);

                    it++;
                    if (it != objit.end())
                    {
                        *out = ',';
                        out++;
                    }
                    else
                    {
                        break;
                    }
                }
            }

            *out = '}';
            out++;
            return out;
        }

        char *c_serialize_array(const json &obj, char *_out)
        {
            char *out = _out;
            *out = '[';
            out++;

            auto arrit = obj.values();
            if (arrit.size())
            {
                for (auto it = arrit.begin();;)
                {
                    out = c_serialize(*it, out);
                    it++;
                    if (it != arrit.end())
                    {
                        *out = ',';
                        out++;
                    }
                    else
                    {
                        break;
                    }
                }
            }

            *out = ']';
            out++;
            return out;
        }

        char *c_serialize(const json &obj, char *_out)
        {
            char *out = _out;
            size_t i64len;
            char i64buf[21];

            switch (obj.type())
            {
            case value_t::integer: {
                char *ptr = fops::i64toa_10(obj.get<int64_t>(), i64buf, i64len);
                memcpy(out, ptr, i64len);
                out += i64len;
            }
            break;

            case value_t::floating: {
                std::string str = std::to_string(obj.get<float>());
                memcpy(out, str.data(), str.size());
                out += str.size();
            }
            break;

            case value_t::string:

                *out = '\"';
                out++;

                {
                    auto view = obj.get<ulib::string_view>();

                    for (const char *it = view.begin().raw(); it != view.end().raw(); it++)
                    {
                        switch (*it)
                        {
                        case '\\':
                            *out = '\\';
                            out++;
                            *out = '\\';
                            out++;
                            break;
                        case '\"':
                            *out = '\\';
                            out++;
                            *out = '\"';
                            out++;
                            break;
                        case '\n':
                            *out = '\\';
                            out++;
                            *out = 'n';
                            out++;
                            break;
                        case '\r':
                            *out = '\\';
                            out++;
                            *out = 'r';
                            out++;
                            break;
                        case '\t':
                            *out = '\\';
                            out++;
                            *out = 't';
                            out++;
                            break;
                        case '\b':
                            *out = '\\';
                            out++;
                            *out = 'b';
                            out++;
                            break;
                        case '\f':
                            *out = '\\';
                            out++;
                            *out = 'f';
                            out++;
                            break;
                        default:
                            *out = *it;
                            out++;
                        }
                    }
                }

                *out = '\"';
                out++;

                break;

            case value_t::object:
                out = c_serialize_object(obj, out);
                break;

            case value_t::array:
                out = c_serialize_array(obj, out);
                break;

            case value_t::boolean:
                if (obj.get<bool>())
                {
                    *out = 't';
                    out++;
                    *out = 'r';
                    out++;
                    *out = 'u';
                    out++;
                    *out = 'e';
                    out++;
                }
                else
                {
                    *out = 'f';
                    out++;
                    *out = 'a';
                    out++;
                    *out = 'l';
                    out++;
                    *out = 's';
                    out++;
                    *out = 'e';
                    out++;
                }
                break;

            default:
                *out = 'n';
                out++;
                *out = 'u';
                out++;
                *out = 'l';
                out++;
                *out = 'l';
                out++;
                break;
            }

            return out;
        }

        void serialize(const json &obj, char *_out)
        {
            size_t size = _out - c_serialize(obj, _out);
            _out[size] = 0;
        }

    } // namespace json_detail

    char *json::c_serialize(const json &obj, char *_out) { return json_detail::c_serialize(obj, _out); }
    size_t json::serialized_length(const json &obj) { return json_detail::serialized_length(obj); }

} // namespace ulib
