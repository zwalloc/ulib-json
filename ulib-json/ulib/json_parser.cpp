#include "json.h"

namespace ulib
{
    ULIB_RUNTIME_ERROR(ParseError);

    using StringViewT = typename json::StringViewT;
    using StringT = typename json::StringT;
    using value_t = typename json::value_t;

    json json::parser::parse(ulib::string_view str)
    {
        json obj;
        parse(str, obj);
        return obj;
    }

    void json::parser::parse(ulib::string_view str, json &out)
    {
        set_str(str);

        value_t vt = pending_value();
        parse_value(vt, &out);
    }

    std::pair<int, int> json::parser::error_pos()
    {
        int line = 1;
        int symbol = 1;

        auto it = mBegin;
        auto end = mIt;

        while (it != end)
        {
            char c = *it;
            if (c == '\n')
            {
                symbol = 0;
                line++;
            }
            else
            {
                symbol++;
            }

            it++;
        }

        return std::pair<int, int>(line, symbol);
    }

    void json::parser::step()
    {
        mIt++;
        if (mIt == mEnd)
            throw ParseError{"Unexpected end of file"};
    }

    bool json::parser::step_check_eof()
    {
        mIt++;
        return mIt != mEnd;
    }

    void json::parser::skip_spaces()
    {
        while (mIt != mEnd)
        {
            switch (*mIt)
            {
            case ' ':
            case '\n':
            case '\r':
            case '\t':
                mIt++;
                continue;
            default:
                return;
            }
        }

        throw ParseError{"Unexpected end of file"};
    }

    value_t json::parser::pending_value()
    {
        skip_spaces();

        if (*mIt == '{')
        {
            step();
            skip_spaces();

            return value_t::object;
        }

        if (*mIt == '[')
        {
            step();
            skip_spaces();

            return value_t::array;
        }

        if (*mIt == '\"')
        {
            step();
            return value_t::string;
        }

        if ((*mIt >= '0' && *mIt <= '9') || *mIt == '-')
        {
            return value_t::integer;
        }

        if (*mIt == 'f' || *mIt == 't')
        {
            step();
            return value_t::boolean;
        }

        if (*mIt == 'n')
        {
            step();
            return value_t::null;
        }

        throw ParseError{"json invalid value type"};
    }

    void json::parser::parse_value(value_t vt, json *out)
    {
        switch (vt)
        {
        case value_t::object:
            parse_object(out);
            break;
        case value_t::array:
            parse_array(out);
            break;
        case value_t::integer:
            parse_integer(out);
            break;
        case value_t::string:
            parse_string(out);
            break;
        case value_t::boolean:
            parse_boolean(out);
            break;
        case value_t::null:
            parse_null(out);
            break;
        default:
            throw ParseError{"Unexpected type of value"};
        }
    }

    void json::parser::parse_object(json *out)
    {
        *out = json::object();

        while (true)
        {
            skip_spaces();

            if (*mIt == '\"')
            {
                step();

                // size_t qsize = quote_end_string_size();
                // json::item* itm = out->create_item(qsize);

                auto str = parse_quote_end_string();

                step_through(':');
                value_t vt = pending_value();

                parse_value(vt, &(*out)[str]);

                if (mIt == mEnd)
                    throw ParseError{"Unexpected end of file"};
            }
            else if (*mIt == '}')
            {
                mIt++;
                return;
            }
            else if (*mIt == ',')
            {
                step();
            }
            else
            {
                throw ParseError{"Unexpected character"};
            }
        }
    }

    void json::parser::parse_array(json *out)
    {
        *out = json::array();
        // out->force_array();

        skip_spaces(); // TODO: Check is duplicate
        if (*mIt == ']')
        {
            mIt++;
            return;
        }

        while (true)
        {
            value_t vt = pending_value();
            parse_value(vt, &out->push_back());

            if (mIt == mEnd)
                throw ParseError{"Unexpected end of file"};

            skip_spaces();

            if (*mIt == ']')
            {
                mIt++;
                return;
            }

            if (*mIt == ',')
            {
                step();
            }
        }
    }

    void json::parser::parse_string(json *out) { out->assign(parse_quote_end_string()); }

    void json::parser::parse_integer(json *out)
    {
        bool neg = *mIt == '-';
        if (neg)
        {
            step();
            if (!(*mIt >= '0' && *mIt <= '9'))
                throw ParseError{"Expected integer"};
        }

        int64_t result = *mIt - '0';
        while (step_check_eof())
        {
            if ((*mIt >= '0' && *mIt <= '9'))
                result = result * 10 + (*mIt - '0');
            else if (*mIt == '.')
            {
                float v2 = 0.f;

                do
                {
                    if (!step_check_eof())
                        break;
                    if (!(*mIt >= '0' && *mIt <= '9'))
                        break;

                    float divider = 10.f;

                    v2 += float(*mIt - '0') / divider;
                    divider *= 10.f;

                    while (step_check_eof() && (*mIt >= '0' && *mIt <= '9'))
                    {
                        v2 += float(*mIt - '0') / divider;
                        divider *= 10.f;
                    }

                } while (false);

                float rr = float(result) + v2;
                if (neg)
                    rr = -rr;

                out->assign(rr);
                return;
            }
            else
                break;
        }

        if (neg)
            result = -result;

        out->assign(result);
    }

    void json::parser::parse_boolean(json *out)
    {
        if (*mIt == 'r') // t'r'ue
        {
            step();
            if (*mIt != 'u')
                throw ParseError{"Invalid 'true' constant"};

            step();
            if (*mIt != 'e')
                throw ParseError{"Invalid 'true' constant"};

            out->assign(true);
        }
        else if (*mIt == 'a') // f'a'lse
        {
            step();
            if (*mIt != 'l')
                throw ParseError{"Invalid 'false' constant"};
            step();
            if (*mIt != 's')
                throw ParseError{"Invalid 'false' constant"};
            step();
            if (*mIt != 'e')
                throw ParseError{"Invalid 'false' constant"};

            out->assign(false);
        }
        else
        {
            throw ParseError{"Invalid boolean constant"};
        }

        mIt++;
    }

    void json::parser::parse_null(json *out)
    {
        if (*mIt == 'u') // n'u'll
        {
            step();
            if (*mIt != 'l')
                throw ParseError{"Invalid 'null' constant"};

            step();
            if (*mIt != 'l')
                throw ParseError{"Invalid 'null' constant"};

            // it is already null
            // out->assign(value_t::null);
        }
        else
        {
            throw ParseError{"Invalid 'null' constant"};
        }

        mIt++;
    }

    void json::parser::step_through(char c)
    {
        skip_spaces();
        if (*mIt != c)
            throw ParseError{"Unexpected character"};

        step();
    }

    size_t json::parser::quote_end_string_size()
    {
        auto oldIt = mIt;
        size_t size = 0;

        while (true)
        {
            switch (*mIt)
            {
            case '\"':
                mIt = oldIt;
                return size;
            case '\\':
                step();
                size++;
                break;
            default:
                size++;
                break;
            }

            step();
        }
    }

    StringT json::parser::parse_quote_end_string()
    {
        size_t size = quote_end_string_size();
        StringT result(size);

        auto it = result.data();

        while (true)
        {
            switch (*mIt)
            {
            case '\"':
                mIt++;
                return result;
            case '\\':
                step();

                if (*mIt == 'n')
                    *it = '\n';
                else if (*mIt == 'r')
                    *it = '\r';
                else if (*mIt == 't')
                    *it = '\t';
                else if (*mIt == 'b')
                    *it = '\b';
                else if (*mIt == 'f')
                    *it = '\f';
                else
                    *it = *mIt;
                ++it;
                break;
            default:
                *it = *mIt;
                ++it;
                break;
            }

            step();
        }
    }

    void json::parser::pending_object()
    {
        skip_spaces();
        if (*mIt != '{')
            throw ParseError{"Unexpected character"};
    }

    void json::parser::set_str(ulib::string_view str)
    {
        mIt = str.begin().raw();
        mBegin = str.begin().raw();
        mEnd = str.end().raw();
    }

    // void parse(const std::string &str, json &out)
    // {
    //     parser prsr;
    //     prsr.parse(str, out);
    // }

    // json parse(const std::string &str)
    // {
    //     parser prsr;
    //     return prsr.parse(str);
    // }
} // namespace ulib