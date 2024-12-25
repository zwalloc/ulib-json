#pragma once

#include <ulib/list.h>
#include <ulib/string.h>
#include <ulib/runtimeerror.h>

#include <optional>
#include <filesystem>

namespace ulib
{
    // template <class JsonTy, class T>
    // struct json_iterator : public BaseIterator<T, std::random_access_iterator_tag>
    // {
    //     using JsonT = JsonTy;
    //     using SelfT = json_iterator<JsonT, T>;
    //     using SelfNoConstT = json_iterator<JsonT, std::remove_cv_t<T>>;
    //     using SelfConstT = json_iterator<JsonT, const std::remove_cv_t<T>>;

    //     inline json_iterator() = delete;
    //     inline json_iterator(T *p, size_t offset) : ptr(p) {}

    //     inline json_iterator(const SelfConstT &it) : ptr(it.ptr) {}
    //     inline json_iterator(const SelfNoConstT &it) : ptr(it.ptr) {}

    //     inline T &operator*() const { return *ptr; }

    //     inline SelfT &operator++() { return ptr = (T *)((uchar *)ptr + offset), *this; } // Prefix increment
    //     operator. inline SelfT &operator++(int)
    //     {
    //         return ptr = (T *)((uchar *)ptr + offset), *this;
    //     } // Postfix increment operator.
    //     inline SelfT &operator--() { return ptr = (T *)((uchar *)ptr - offset), *this; }
    //     inline SelfT &operator--(int) { return ptr = (T *)((uchar *)ptr - offset), *this; }
    //     inline SelfT operator+(size_t k) const { return (T *)((uchar *)ptr + offset * k); }
    //     inline SelfT operator-(size_t k) const { return (T *)((uchar *)ptr - offset * k); }
    //     inline SelfT operator+=(size_t k) { return ptr = (T *)((uchar *)ptr + offset * k); }
    //     inline SelfT operator-=(size_t k) { return ptr = (T *)((uchar *)ptr - offset * k); }

    //     inline bool operator>(SelfNoConstT k) const { return ptr > k.ptr; }
    //     inline bool operator<(SelfNoConstT k) const { return ptr < k.ptr; }
    //     inline bool operator<=(SelfNoConstT k) const { return ptr <= k.ptr; }
    //     inline bool operator>=(SelfNoConstT k) const { return ptr >= k.ptr; }
    //     inline bool operator==(SelfNoConstT k) const { return ptr == k.ptr; }
    //     inline bool operator!=(SelfNoConstT k) const { return ptr != k.ptr; }
    //     inline size_t operator-(SelfNoConstT k) const { return ptr - k.ptr; }

    //     inline bool operator>(SelfConstT k) const { return ptr > k.ptr; }
    //     inline bool operator<(SelfConstT k) const { return ptr < k.ptr; }
    //     inline bool operator<=(SelfConstT k) const { return ptr <= k.ptr; }
    //     inline bool operator>=(SelfConstT k) const { return ptr >= k.ptr; }
    //     inline bool operator==(SelfConstT k) const { return ptr == k.ptr; }
    //     inline bool operator!=(SelfConstT k) const { return ptr != k.ptr; }
    //     inline size_t operator-(SelfConstT k) const { return ptr - k.ptr; }

    //     inline T *operator->() const { return ptr; }

    //     // inline T *Raw() { return ptr; }
    //     // inline T *raw() { return ptr; }
    // private:
    //     JsonT* mParent;
    //     size_t mIndex;
    // };

    class json
    {
    public:
        ULIB_RUNTIME_ERROR(exception);

        template <class JsonTy>
        class basic_item : public JsonTy
        {
        public:
            using JsonT = JsonTy;
            using ThisT = basic_item<JsonT>;
            using StringT = typename JsonT::StringT;
            using StringViewT = typename JsonT::StringViewT;

            basic_item() : JsonT(), mName() {}
            basic_item(const basic_item &other) : JsonT(other), mName(other.mName) {}
            basic_item(StringViewT name) : JsonT(), mName(name) {}
            ~basic_item() {}

            // ulib::string_view name() { return this->name(); }
            StringViewT name() const { return mName; }
            JsonT &value() { return *this; }
            const JsonT &value() const { return *this; }

        private:
            StringT mName;
        };

        enum class value_t
        {
            null,
            integer,
            string,
            array,
            object,
            floating,
            boolean
        };

        using ThisT = ulib::json;
        using EncodingT = ulib::MultibyteEncoding;
        using CharT = typename EncodingT::CharT;
        using AllocatorT = ulib::DefaultAllocator;
        using StringT = ulib::EncodedString<EncodingT, AllocatorT>;
        using StringViewT = ulib::EncodedStringView<EncodingT>;

        using ItemT = basic_item<ulib::json>;
        using ObjectT = ulib::List<ItemT, AllocatorT>;
        using ArrayT = ulib::List<ThisT, AllocatorT>;

        using Iterator = ulib::RandomAccessIterator<ThisT>;
        using ConstIterator = ulib::RandomAccessIterator<const ThisT>;

        using iterator = Iterator;
        using const_iterator = ConstIterator;
        using value_type = ThisT;
        using pointer = ThisT *;
        using reference = ThisT &;
        using const_reference = const ThisT &;

        static StringViewT type_to_string(value_t t)
        {
            switch (t)
            {
            case value_t::null:
                return "null";
            case value_t::integer:
                return "integer";
            case value_t::floating:
                return "floating";
            case value_t::boolean:
                return "boolean";
            case value_t::array:
                return "array";
            case value_t::object:
                return "object";
            case value_t::string:
                return "string";
            }

            return "unknown";
        }

        static json object() { return json{value_t::object}; }
        static json array() { return json{value_t::array}; }

        class parser
        {
        public:
            json parse(ulib::string_view str);
            void parse(ulib::string_view str, json &out);
            // line, symbol
            std::pair<int, int> error_pos();

        private:
            void skip_spaces();
            void step();
            bool step_check_eof();

            value_t pending_value();

            void parse_value(value_t vt, json *out);

            void parse_object(json *out);
            void parse_array(json *out);
            void parse_string(json *out);
            void parse_integer(json *out);
            void parse_boolean(json *out);
            void parse_null(json *out);

            void step_through(char c);

            size_t quote_end_string_size();
            StringT parse_quote_end_string();

            void pending_object();

            void set_str(ulib::string_view str);

            const char *mIt;
            const char *mBegin;
            const char *mEnd;
        };

        struct vtable
        {
            size_t (*size)(json *t);
            iterator (*begin)(json *t);
            iterator (*end)(json *t);
            iterator (*erase)(json *t, iterator);
            iterator (*find)(json *t, iterator);

            // TODO: more
        };

        static json parse(StringViewT str)
        {
            parser prsr;
            return prsr.parse(str);
        }

        json() : mType(value_t::null) {}
        json(const json &v);
        json(json &&v);

        json(value_t t);

        template <class T, class TEncodingT = argument_encoding_or_die_t<T>>
        json(const T &v)
        {
            construct_as_string(StringViewT{ulib::str(ulib::u8(v))});
        }

        template <class T, std::enable_if_t<std::is_same_v<T, StringT>, bool> = true>
        json(T &&v)
        {
            move_construct_as_string(std::move(v));
        }

        template <class T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
        json(T v) : mType(value_t::floating), mFloatVal(float(v))
        {
        }

        template <class T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, bool> = true>
        json(T v) : mType(value_t::integer), mIntVal(int64_t(v))
        {
        }

        template <class T, std::enable_if_t<std::is_same_v<T, bool>, bool> = true>
        json(T v) : mType(value_t::boolean), mBoolVal(v)
        {
        }

        ~json();

        template <class T, class TEncodingT = argument_encoding_or_die_t<T>>
        void assign(const T &right)
        {
            implicit_set_string(ulib::str(ulib::u8(right)));
        }

        void assign(const std::filesystem::path &right)
        {
            implicit_set_string(ulib::str(ulib::u8(right.generic_u8string())));
        }

        void assign(value_t t) { implicit_set_type(t); }

        template <class T, std::enable_if_t<std::is_same_v<T, StringT>, bool> = true>
        void assign(T &&right)
        {
            implicit_move_set_string(std::move(right));
        }

        template <class T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
        void assign(T v)
        {
            implicit_set_float(float(v));
        }

        template <class T, std::enable_if_t<std::is_same_v<T, bool>, bool> = true>
        void assign(T v)
        {
            implicit_set_boolean(int64_t(v));
        }

        template <class T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, bool> = true>
        void assign(T v)
        {
            implicit_set_integer(int64_t(v));
        }

        template <class T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
        T get() const
        {
            if (mType == value_t::floating)
                return T(mFloatVal);
            if (mType == value_t::integer)
                return T(mIntVal);

            throw json::exception(ulib::string{"json invalid get() type. expected: floating or integer. current: "} +
                                  type_to_string(mType));
        }

        template <class T, std::enable_if_t<std::is_same_v<T, bool>, bool> = true>
        T get() const
        {
            if (mType == value_t::boolean)
                return T(mBoolVal);

            throw json::exception(ulib::string{"json invalid get() type. expected: boolean. current: "} +
                                  type_to_string(mType));
        }

        template <class T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, bool> = true>
        T get() const
        {
            if (mType == value_t::integer)
                return T(mIntVal);
            if (mType == value_t::floating)
                return T(mIntVal);

            throw json::exception(ulib::string{"json invalid get() type. expected: integer or floating. current: "} +
                                  type_to_string(mType));
        }

        template <class T, class VT = typename T::value_type, class TEncodingT = argument_encoding_or_die_t<T>,
                  std::enable_if_t<is_string_v<T>, bool> = true>
        T get() const
        {
            if (mType == value_t::string)
                return ulib::Convert<TEncodingT>(ulib::u8(mString));

            throw json::exception(ulib::string{"json invalid get() type. expected: string. current: "} +
                                  type_to_string(mType));
        }

        template <
            class T, class VT = typename T::value_type, class TEncodingT = argument_encoding_or_die_t<T>,
            std::enable_if_t<is_string_view_v<T> && is_encodings_raw_movable_v<EncodingT, TEncodingT>, bool> = true>
        T get() const
        {
            if (mType == value_t::string)
                return ulib::string_view{mString.raw_data(), mString.size()};

            throw json::exception(ulib::string{"json invalid get() type. expected: string. current: "} +
                                  type_to_string(mType));
        }

        template <class T, std::enable_if_t<std::is_same_v<T, std::filesystem::path>, bool> = true>
        T get() const
        {
            if (mType == value_t::string)
                return ulib::string_view{mString.raw_data(), mString.size()};

            throw json::exception(ulib::string{"json invalid get() type. expected: string. current: "} +
                                  type_to_string(mType));
        }

        template <class T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
        reference operator=(T right)
        {
            return assign(right), *this;
        }

        template <class T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
        reference operator=(T right)
        {
            return assign(right), *this;
        }

        template <class T, class VT = typename T::value_type, std::enable_if_t<ulib::is_string_kind_v<T>, bool> = true>
        reference operator=(const T &right)
        {
            return assign(ulib::str(ulib::u8(right))), *this;
        }

        template <class T, class KEncodingT = literal_encoding_t<T>,
                  std::enable_if_t<!std::is_same_v<KEncodingT, missing_type>, bool> = true>
        reference operator=(const T *right)
        {
            return assign(right), *this;
        }

        reference operator=(const std::filesystem::path &path) { return assign(path), *this; }
        reference operator=(value_t t) { return assign(t), *this; }

        reference operator=(const_reference right);
        reference operator=(json &&right);

        template <class T>
        void assign(const std::optional<T> &right)
        {
            if (right)
                assign(*right);
            else
                mType = value_t::null;
        }

        template <class T>
        std::optional<T> try_get() const
        {
            if (mType == value_t::null)
                return std::nullopt;

            return get<T>();
        }

        template <class T>
        reference operator=(const std::optional<T> &right)
        {
            return assign<std::optional<T>>(right), *this;
        }

        // if value is exists, works like "at" otherwise creates value and set value type to undefined
        reference find_or_create(StringViewT name);
        reference find_or_create(size_t idx);

        const_reference find_if_exists(StringViewT name) const;
        const_reference find_if_exists(size_t idx) const;

        reference at(StringViewT key) { return reference(find_if_exists(key)); }
        reference at(size_t idx) { return reference(find_if_exists(idx)); }

        const_reference at(StringViewT key) const { return find_if_exists(key); }
        const_reference at(size_t idx) const { return find_if_exists(idx); }

        reference operator[](StringViewT key) { return find_or_create(key); }
        reference operator[](size_t idx) { return find_or_create(idx); }

        const_reference operator[](StringViewT key) const { return at(key); }
        const_reference operator[](size_t idx) const { return at(idx); }

        span<const ItemT> items() const { return implicit_const_touch_object(), mObject; }
        span<ItemT> items() { return implicit_touch_object(), mObject; }

        span<const json> values() const { return implicit_const_touch_array(), mArray; }
        span<json> values() { return implicit_touch_array(), mArray; }

        iterator begin() { return implicit_const_touch_array(), mArray.begin(); }
        const_iterator begin() const { return implicit_const_touch_array(), mArray.begin(); }

        iterator end() { return implicit_const_touch_array(), mArray.end(); }
        const_iterator end() const { return implicit_const_touch_array(), mArray.end(); }

        const json *search(StringViewT name) const
        {
            if (mType != value_t::object)
                throw json::exception(ulib::string{"failed to search via key: \""} + name +
                                      "\" json value must be an object. current: " + type_to_string(mType));

            return find_object_in_object(name);
        }

        json *search(StringViewT name)
        {
            if (mType != value_t::object)
                throw json::exception(ulib::string{"failed to search via key: \""} + name +
                                      "\" json value must be an object. current: " + type_to_string(mType));

            return find_object_in_object(name);
        }

        size_t size() const { return values().size(); }
        reference push_back();
        value_t type() const { return mType; }

        template <class TStringT = ulib::string, class TEncodingT = string_encoding_t<TStringT>,
                  std::enable_if_t<!std::is_same_v<TEncodingT, missing_type> && is_string_v<TStringT>, bool> = true>
        TStringT dump() const
        {
            size_t len = serialized_length(*this);
            StringT result(len);
            c_serialize(*this, (char *)&result[0]);
            return ulib::Convert<TEncodingT>(ulib::u8(result));
        }

        inline void remove(StringViewT key)
        {
            if (mType != value_t::object)
                throw json::exception(ulib::string{"failed to remove key: \""} + key +
                                      "\" json value must be an object. current: " + type_to_string(mType));

            for (auto it = mObject.begin(); it != mObject.end(); it++)
            {
                if (it->name() == key)
                {
                    mObject.erase(it);
                    return;
                }
            }
        }

        inline bool is_int() const { return mType == value_t::integer; }
        inline bool is_float() const { return mType == value_t::floating; }
        inline bool is_string() const { return mType == value_t::string; }
        inline bool is_array() const { return mType == value_t::array; }
        inline bool is_object() const { return mType == value_t::object; }
        inline bool is_number() const { return mType == value_t::integer || mType == value_t::floating; }
        inline bool is_bool() const { return mType == value_t::boolean; }
        inline bool is_null() const { return mType == value_t::null; }

    private:
        void initialize_as_string();
        void initialize_as_object();
        void initialize_as_array();

        bool implicit_touch_string();
        bool implicit_touch_object();
        bool implicit_touch_array();

        void implicit_const_touch_string() const;
        void implicit_const_touch_object() const;
        void implicit_const_touch_array() const;

        void implicit_set_type(value_t t);
        void implicit_set_string(StringViewT other);
        void implicit_move_set_string(StringT &&other);
        void implicit_set_float(float other);
        void implicit_set_integer(int64_t other);
        void implicit_set_boolean(bool other);

        void construct_from_type(value_t t);

        void move_construct_as_string(StringT &&other);
        void construct_as_string(StringViewT other);

        void copy_construct_from_other(const json &other);
        void move_construct_from_other(json &&other);

        void destroy_containers();

        json *find_object_in_object(StringViewT name);
        const json *find_object_in_object(StringViewT name) const;

        value_t mType;

        union {
            bool mBoolVal;
            float mFloatVal;
            int64_t mIntVal;

            StringT mString;
            ObjectT mObject;
            ArrayT mArray;
        };

        static size_t serialized_length(const json &obj);
        static char *c_serialize(const json &obj, char *_out);
    };

} // namespace ulib